/***************************************************************************
 *   Copyright (C) 2005 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *                                                                         *
 *   Copyright (C) 2007-2010 Øyvind Harboe                                 *
 *   oyvind.harboe@zylin.com                                               *
 *                                                                         *
 *   Copyright (C) 2008 by Spencer Oliver                                  *
 *   spen@spen-soft.co.uk                                                  *
 *                                                                         *
 *   Copyright (C) 2011 by Broadcom Corporation                            *
 *   Evan Hunter - ehunter@broadcom.com                                    *
 *                                                                         *
 *   Copyright (C) ST-Ericsson SA 2011                                     *
 *   michel.jaouen@stericsson.com : smp minimum support                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <target/breakpoints.h>
#include <target/target_request.h>
#include <target/target_type.h>
#include <target/register.h>
#include "server.h"
// #include <flash/nor/core.h>
#include "gdb_server.h"
// #include <target/image.h>
#include <target/mapu_spu.h>
#include <jtag/jtag.h>
// #include "rtos/rtos.h"
// #include "target/smp.h"
#include <helper/replacements.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

/**
 * @file
 * GDB server implementation.
 *
 * This implements the GDB Remote Serial Protocol, over TCP connections,
 * giving GDB access to the JTAG or other hardware debugging facilities
 * found in most modern embedded processors.
 */

/* private connection data for GDB.
 * naming it gdb_info would be better */
struct gdb_connection
{
	char buffer[GDB_BUFFER_SIZE];
	char *buf_p;
	int buf_cnt;
	int ctrl_c;
	enum target_state frontend_state;
	struct image *vflash_image;
	int closed;
	int busy;
	int noack_mode;
	/* 	set flag to true if you want the next stepi to return immediately.
		allowing GDB to pick up a fresh set of register values from the target
		without modifying the target state. */
	bool sync;
	/* We delay reporting memory write errors until next step/continue or memory
	 * write. This improves performance of gdb load significantly as the GDB packet
	 * can be replied immediately and a new GDB packet will be ready without delay
	 * (ca. 10% or so...).
	 */
	bool mem_write_error;
};

#if 0
#define _DEBUG_GDB_IO_
#endif

extern struct target *all_targets;

static struct gdb_connection *current_gdb_connection;

static int gdb_breakpoint_override;
static enum breakpoint_type gdb_breakpoint_override_type;

static int gdb_error(struct connection *connection, int retval);
static int gdb_input(struct connection *connection);

static const char *gdb_port;
static const char *gdb_port_next;
static const char DIGITS[16] = "0123456789abcdef";

static void gdb_log_callback(void *priv, const char *file, unsigned line,
                             const char *function, const char *string);

/* number of gdb connections, mainly to suppress gdb related debugging spam
 * in helper/log.c when no gdb connections are actually active */
int gdb_actual_connections;

/* set if we are sending a memory map to gdb
 * via qXfer:memory-map:read packet */
/* enabled by default*/
static int gdb_use_memory_map = 1;
/* enabled by default*/
static int gdb_flash_program = 1;

/* if set, data aborts cause an error to be reported in memory read packets
 * see the code in gdb_read_memory_packet() for further explanations.
 * Disabled by default.
 */
static int gdb_report_data_abort;

static int gdb_last_signal(struct target *target)
{
	switch (target->debug_reason)
	{
		case DBG_REASON_DBGRQ:
			return 0x2; /* SIGINT */

		case DBG_REASON_BREAKPOINT:
		case DBG_REASON_WATCHPOINT:

		case DBG_REASON_WPTANDBKPT:

		case DBG_REASON_SINGLESTEP:
			return 0x05; /* SIGTRAP */

		case DBG_REASON_NOTHALTED:
			return 0x0; /* no signal... shouldn't happen */

		default:
			LOG_USER("undefined debug reason %d - target needs reset", target->debug_reason);
			return 0x0;
	}
}

static int check_pending(struct connection *connection,
		int timeout_s, int *got_data)
{
	/* a non-blocking socket will block if there is 0 bytes available on the socket,
	 * but return with as many bytes as are available immediately
	 */
	struct timeval tv;
	fd_set read_fds;
	struct gdb_connection *gdb_con = connection->priv;
	int t;
	if (got_data == NULL)
		got_data=&t;
	*got_data = 0;

	if (gdb_con->buf_cnt > 0)
	{
		*got_data = 1;
		return ERROR_OK;
	}

	FD_ZERO(&read_fds);
	FD_SET(connection->fd, &read_fds);

	tv.tv_sec = timeout_s;
	tv.tv_usec = 0;
	if (socket_select(connection->fd + 1, &read_fds, NULL, NULL, &tv) == 0)
	{
		/* This can typically be because a "monitor" command took too long
		 * before printing any progress messages
		 */
		if (timeout_s > 0)
		{
			return ERROR_GDB_TIMEOUT;
		} else
		{
			return ERROR_OK;
		}
	}
	*got_data = FD_ISSET(connection->fd, &read_fds) != 0;
	return ERROR_OK;
}

static int gdb_get_char_inner(struct connection *connection, int* next_char)
{
	struct gdb_connection *gdb_con = connection->priv;
	int retval = ERROR_OK;

#ifdef _DEBUG_GDB_IO_
	char *debug_buffer;
#endif
	for (;;)
	{
		if (connection->service->type != CONNECTION_TCP)
		{
			gdb_con->buf_cnt = connection_read(connection, gdb_con->buffer, GDB_BUFFER_SIZE);
		}
		else
		{
			retval = check_pending(connection, 1, NULL);
			if (retval != ERROR_OK)
				return retval;

			gdb_con->buf_cnt = connection_read(connection, gdb_con->buffer, GDB_BUFFER_SIZE);
		}

		if (gdb_con->buf_cnt > 0)
		{
			break;
		}
		if (gdb_con->buf_cnt == 0)
		{
			gdb_con->closed = 1;
			return ERROR_SERVER_REMOTE_CLOSED;
		}

#ifdef _WIN32
		errno = WSAGetLastError();

		switch (errno)
		{
			case WSAEWOULDBLOCK:
				usleep(1000);
				break;
			case WSAECONNABORTED:
				gdb_con->closed = 1;
				return ERROR_SERVER_REMOTE_CLOSED;
			case WSAECONNRESET:
				gdb_con->closed = 1;
				return ERROR_SERVER_REMOTE_CLOSED;
			default:
				LOG_ERROR("read: %d", errno);
				exit(-1);
		}
#else
		switch (errno)
		{
			case EAGAIN:
				usleep(1000);
				break;
			case ECONNABORTED:
				gdb_con->closed = 1;
				return ERROR_SERVER_REMOTE_CLOSED;
			case ECONNRESET:
				gdb_con->closed = 1;
				return ERROR_SERVER_REMOTE_CLOSED;
			default:
				LOG_ERROR("read: %s", strerror(errno));
				gdb_con->closed = 1;
				return ERROR_SERVER_REMOTE_CLOSED;
		}
#endif
	}

#ifdef _DEBUG_GDB_IO_
	debug_buffer = malloc(gdb_con->buf_cnt + 1);
	memcpy(debug_buffer, gdb_con->buffer, gdb_con->buf_cnt);
	debug_buffer[gdb_con->buf_cnt] = 0;
	LOG_DEBUG("received '%s'", debug_buffer);
	free(debug_buffer);
#endif

	gdb_con->buf_p = gdb_con->buffer;
	gdb_con->buf_cnt--;
	*next_char = *(gdb_con->buf_p++);

	if (gdb_con->buf_cnt > 0)
		connection->input_pending = 1;
	else
		connection->input_pending = 0;

#ifdef _DEBUG_GDB_IO_
	LOG_DEBUG("returned char '%c' (0x%2.2x)", *next_char, *next_char);
#endif

	return retval;
}

/**
 * The cool thing about this fn is that it allows buf_p and buf_cnt to be
 * held in registers in the inner loop.
 *
 * For small caches and embedded systems this is important!
 */
static inline int gdb_get_char_fast(struct connection *connection, int* next_char, char **buf_p, int *buf_cnt)
{
	int retval = ERROR_OK;

	if ((*buf_cnt)-- > 0)
	{
		*next_char = **buf_p;
		(*buf_p)++;
		if (*buf_cnt > 0)
			connection->input_pending = 1;
		else
			connection->input_pending = 0;

#ifdef _DEBUG_GDB_IO_
		LOG_DEBUG("returned char '%c' (0x%2.2x)", *next_char, *next_char);
#endif

		return ERROR_OK;
	}

	struct gdb_connection *gdb_con = connection->priv;
	gdb_con->buf_p = *buf_p;
	gdb_con->buf_cnt = *buf_cnt;
	retval = gdb_get_char_inner(connection, next_char);
	*buf_p = gdb_con->buf_p;
	*buf_cnt = gdb_con->buf_cnt;

	return retval;
}


static int gdb_get_char(struct connection *connection, int* next_char)
{
	struct gdb_connection *gdb_con = connection->priv;
	return gdb_get_char_fast(connection, next_char, &gdb_con->buf_p, &gdb_con->buf_cnt);
}


static int gdb_putback_char(struct connection *connection, int last_char)
{
	struct gdb_connection *gdb_con = connection->priv;

	if (gdb_con->buf_p > gdb_con->buffer)
	{
		*(--gdb_con->buf_p) = last_char;
		gdb_con->buf_cnt++;
	}
	else
	{
		LOG_ERROR("BUG: couldn't put character back");
	}

	return ERROR_OK;
}

/* The only way we can detect that the socket is closed is the first time
 * we write to it, we will fail. Subsequent write operations will
 * succeed. Shudder! */
static int gdb_write(struct connection *connection, void *data, int len)
{
	struct gdb_connection *gdb_con = connection->priv;
	if (gdb_con->closed)
		return ERROR_SERVER_REMOTE_CLOSED;

	if (connection_write(connection, data, len) == len)
	{
	  // dump what gdb sends
	  /*char* ptr = strndup(data, len);
	  char* content = ptr? ptr:data;

	  LOG_INFO("gdb server (%d) sends (length %d): %s",
             connection->service->portnumber, len, content);

	  if(ptr) free(ptr);*/

		return ERROR_OK;
	}
	gdb_con->closed = 1;
	return ERROR_SERVER_REMOTE_CLOSED;
}

static int gdb_put_packet_inner(struct connection *connection,
		char *buffer, int len)
{
	int i;
	unsigned char my_checksum = 0;
#ifdef _DEBUG_GDB_IO_
	char *debug_buffer;
#endif
	int reply;
	int retval;
	struct gdb_connection *gdb_con = connection->priv;

	for (i = 0; i < len; i++)
		my_checksum += buffer[i];

#ifdef _DEBUG_GDB_IO_
	/*
	 * At this point we should have nothing in the input queue from GDB,
	 * however sometimes '-' is sent even though we've already received
	 * an ACK (+) for everything we've sent off.
	 */
	int gotdata;
	for (;;)
	{
		retval = check_pending(connection, 0, &gotdata);
		if (retval != ERROR_OK)
			return retval;
		if (!gotdata)
			break;
		if ((retval = gdb_get_char(connection, &reply)) != ERROR_OK)
			return retval;
		if (reply == '$') {
			/* fix a problem with some IAR tools */
			gdb_putback_char(connection, reply);
			LOG_DEBUG("Unexpected start of new packet");
			break;
		}

		LOG_WARNING("Discard unexpected char %c", reply);
	}
#endif

	while (1)
	{
#ifdef _DEBUG_GDB_IO_
		debug_buffer = malloc(len + 1);
		memcpy(debug_buffer, buffer, len);
		debug_buffer[len] = 0;
		LOG_DEBUG("sending packet '$%s#%2.2x'", debug_buffer, my_checksum);
		free(debug_buffer);
#endif

		char local_buffer[1024];
		local_buffer[0] = '$';
		if ((size_t)len + 4 <= sizeof(local_buffer))
		{
			/* performance gain on smaller packets by only a single call to gdb_write() */
			memcpy(local_buffer + 1, buffer, len++);
			local_buffer[len++] = '#';
			local_buffer[len++] = DIGITS[(my_checksum >> 4) & 0xf];
			local_buffer[len++] = DIGITS[my_checksum & 0xf];
			if ((retval = gdb_write(connection, local_buffer, len)) != ERROR_OK)
			{
				return retval;
			}
		}
		else
		{
			/* larger packets are transmitted directly from caller supplied buffer
			   by several calls to gdb_write() to avoid dynamic allocation */
			local_buffer[1] = '#';
			local_buffer[2] = DIGITS[(my_checksum >> 4) & 0xf];
			local_buffer[3] = DIGITS[my_checksum & 0xf];
			if ((retval = gdb_write(connection, local_buffer, 1)) != ERROR_OK)
			{
				return retval;
			}
			if ((retval = gdb_write(connection, buffer, len)) != ERROR_OK)
			{
				return retval;
			}
			if ((retval = gdb_write(connection, local_buffer + 1, 3)) != ERROR_OK)
			{
				return retval;
			}
		}

		if (gdb_con->noack_mode)
			break;

		if ((retval = gdb_get_char(connection, &reply)) != ERROR_OK)
			return retval;

		if (reply == '+')
			break;
		else if (reply == '-')
		{
			/* Stop sending output packets for now */
			log_remove_callback(gdb_log_callback, connection);
			LOG_WARNING("negative reply, retrying");
		}
		else if (reply == 0x3)
		{
			gdb_con->ctrl_c = 1;
			if ((retval = gdb_get_char(connection, &reply)) != ERROR_OK)
				return retval;
			if (reply == '+')
				break;
			else if (reply == '-')
			{
				/* Stop sending output packets for now */
				log_remove_callback(gdb_log_callback, connection);
				LOG_WARNING("negative reply, retrying");
			}
			else if (reply == '$') {
				LOG_ERROR("GDB missing ack(1) - assumed good");
				gdb_putback_char(connection, reply);
				return ERROR_OK;
			} else {

				LOG_ERROR("unknown character(1) 0x%2.2x in reply, dropping connection", reply);
				gdb_con->closed = 1;
				return ERROR_SERVER_REMOTE_CLOSED;
			}
		}
		else if (reply == '$') {
			LOG_ERROR("GDB missing ack(2) - assumed good");
			gdb_putback_char(connection, reply);
			return ERROR_OK;
		}
		else
		{
			LOG_ERROR("unknown character(2) 0x%2.2x in reply, dropping connection", reply);
			gdb_con->closed = 1;
			return ERROR_SERVER_REMOTE_CLOSED;
		}
	}
	if (gdb_con->closed)
		return ERROR_SERVER_REMOTE_CLOSED;

	return ERROR_OK;
}

int gdb_put_packet(struct connection *connection, char *buffer, int len)
{
	struct gdb_connection *gdb_con = connection->priv;
	gdb_con->busy = 1;
	int retval = gdb_put_packet_inner(connection, buffer, len);
	gdb_con->busy = 0;

	/* we sent some data, reset timer for keep alive messages */
	kept_alive();

  char* ptr = strndup(buffer, len);
  char* content = ptr? ptr:buffer;

	if(retval == ERROR_OK)
	  LOG_DEBUG("gdb server (%d) puts packet (length %d): %s",
	           connection->service->portnumber, len, content);
	else
	  LOG_INFO("gdb server (%d) error(%d) in putting packet (length %d): %s",
	           connection->service->portnumber, retval, len, content);

	if(ptr) free(ptr);

	return retval;
}

static __inline__ int fetch_packet(struct connection *connection, int *checksum_ok, int noack, int *len, char *buffer)
{
	unsigned char my_checksum = 0;
	char checksum[3];
	int character;
	int retval = ERROR_OK;

	struct gdb_connection *gdb_con = connection->priv;
	my_checksum = 0;
	int count = 0;
	count = 0;

	/* move this over into local variables to use registers and give the
	 * more freedom to optimize */
	char *buf_p = gdb_con->buf_p;
	int buf_cnt = gdb_con->buf_cnt;

	for (;;)
	{
		/* The common case is that we have an entire packet with no escape chars.
		 * We need to leave at least 2 bytes in the buffer to have
		 * gdb_get_char() update various bits and bobs correctly.
		 */
		if ((buf_cnt > 2) && ((buf_cnt + count) < *len))
		{
			/* The compiler will struggle a bit with constant propagation and
			 * aliasing, so we help it by showing that these values do not
			 * change inside the loop
			 */
			int i;
			char *buf = buf_p;
			int run = buf_cnt - 2;
			i = 0;
			int done = 0;
			while (i < run)
			{
				character = *buf++;
				i++;
				if (character == '#')
				{
					/* Danger! character can be '#' when esc is
					 * used so we need an explicit boolean for done here.
					 */
					done = 1;
					break;
				}

				if (character == '}')
				{
					/* data transmitted in binary mode (X packet)
					 * uses 0x7d as escape character */
					my_checksum += character & 0xff;
					character = *buf++;
					i++;
					my_checksum += character & 0xff;
					buffer[count++] = (character ^ 0x20) & 0xff;
				}
				else
				{
					my_checksum += character & 0xff;
					buffer[count++] = character & 0xff;
				}
			}
			buf_p += i;
			buf_cnt -= i;
			if (done)
				break;
		}
		if (count > *len)
		{
			LOG_ERROR("packet buffer too small");
			retval = ERROR_GDB_BUFFER_TOO_SMALL;
			break;
		}

		retval = gdb_get_char_fast(connection, &character, &buf_p, &buf_cnt);
		if (retval != ERROR_OK)
			break;

		if (character == '#')
			break;

		if (character == '}')
		{
			/* data transmitted in binary mode (X packet)
			 * uses 0x7d as escape character */
			my_checksum += character & 0xff;

			retval = gdb_get_char_fast(connection, &character, &buf_p, &buf_cnt);
			if (retval != ERROR_OK)
				break;

			my_checksum += character & 0xff;
			buffer[count++] = (character ^ 0x20) & 0xff;
		}
		else
		{
			my_checksum += character & 0xff;
			buffer[count++] = character & 0xff;
		}
	}

	gdb_con->buf_p = buf_p;
	gdb_con->buf_cnt = buf_cnt;

	if (retval != ERROR_OK)
		return retval;

	*len = count;

	if ((retval = gdb_get_char(connection, &character)) != ERROR_OK)
		return retval;
	checksum[0] = character;
	if ((retval = gdb_get_char(connection, &character)) != ERROR_OK)
		return retval;
	checksum[1] = character;
	checksum[2] = 0;

	if (!noack)
	{
		*checksum_ok = (my_checksum == strtoul(checksum, NULL, 16));
	}

	return ERROR_OK;
}

static int gdb_get_packet_inner(struct connection *connection,
		char *buffer, int *len)
{
	int character;
	int retval;
	struct gdb_connection *gdb_con = connection->priv;

	while (1)
	{
		do {
			if ((retval = gdb_get_char(connection, &character)) != ERROR_OK)
				return retval;

#ifdef _DEBUG_GDB_IO_
			LOG_DEBUG("character: '%c'", character);
#endif

			switch (character)
			{
				case '$':
					break;
				case '+':
					/* gdb sends a dummy ack '+' at every remote connect - see remote_start_remote (remote.c)
					 * in case anyone tries to debug why they receive this warning every time */
					LOG_WARNING("acknowledgment received, but no packet pending");
					break;
				case '-':
					LOG_WARNING("negative acknowledgment, but no packet pending");
					break;
				case 0x3:
					gdb_con->ctrl_c = 1;
					*len = 0;
					return ERROR_OK;
				default:
					LOG_WARNING("ignoring character 0x%x", character);
					break;
			}
		} while (character != '$');



		int checksum_ok = 0;
		/* explicit code expansion here to get faster inlined code in -O3 by not
		 * calculating checksum
		 */
		if (gdb_con->noack_mode)
		{
			if ((retval = fetch_packet(connection, &checksum_ok, 1, len, buffer)) != ERROR_OK)
				return retval;
		} else
		{
			if ((retval = fetch_packet(connection, &checksum_ok, 0, len, buffer)) != ERROR_OK)
				return retval;
		}

		if (gdb_con->noack_mode)
		{
			/* checksum is not checked in noack mode */
			break;
		}
		if (checksum_ok)
		{
			if ((retval = gdb_write(connection, "+", 1)) != ERROR_OK)
			{
				return retval;
			}
			break;
		}
	}
	if (gdb_con->closed)
		return ERROR_SERVER_REMOTE_CLOSED;

	return ERROR_OK;
}

static int gdb_get_packet(struct connection *connection, char *buffer, int *len)
{
	struct gdb_connection *gdb_con = connection->priv;
	gdb_con->busy = 1;
	int retval = gdb_get_packet_inner(connection, buffer, len);
	gdb_con->busy = 0;

	keep_alive();

  char* ptr = strndup(buffer, *len);
  if(retval == ERROR_OK) {
    if(ptr) {
      LOG_DEBUG("gdb server (%d) gets packet (length %d): %s",
               connection->service->portnumber, *len, ptr);
      free(ptr);
    }
  }
  else
    LOG_INFO("gdb server (%d) error(%d) in getting packet",
             connection->service->portnumber, retval);

	return retval;
}

static int gdb_output_con(struct connection *connection, const char* line)
{
	char *hex_buffer;
	int i, bin_size;

	bin_size = strlen(line);

	hex_buffer = malloc(bin_size*2 + 2);
	if (hex_buffer == NULL)
		return ERROR_GDB_BUFFER_TOO_SMALL;

	hex_buffer[0] = 'O';
	for (i = 0; i < bin_size; i++)
		snprintf(hex_buffer + 1 + i*2, 3, "%2.2x", line[i]);
	hex_buffer[bin_size*2 + 1] = 0;

	int retval = gdb_put_packet(connection, hex_buffer, bin_size*2 + 1);

	free(hex_buffer);
	return retval;
}

static int gdb_output(struct command_context *context, const char* line)
{
	/* this will be dumped to the log and also sent as an O packet if possible */
	LOG_USER_N("%s", line);
	return ERROR_OK;
}


static void gdb_frontend_halted(struct target *target, struct connection *connection)
{
	struct gdb_connection *gdb_connection = connection->priv;

	/* In the GDB protocol when we are stepping or continuing execution,
	 * we have a lingering reply. Upon receiving a halted event
	 * when we have that lingering packet, we reply to the original
	 * step or continue packet.
	 *
	 * Executing monitor commands can bring the target in and
	 * out of the running state so we'll see lots of TARGET_EVENT_XXX
	 * that are to be ignored.
	 */
	if (gdb_connection->frontend_state == TARGET_RUNNING)
	{
		char sig_reply[4];
		int signal_var;

		/* stop forwarding log packets! */
		log_remove_callback(gdb_log_callback, connection);

		if (gdb_connection->ctrl_c)
		{
			signal_var = 0x2;
			gdb_connection->ctrl_c = 0;
		}
		else
		{
			signal_var = gdb_last_signal(target);
		}

		snprintf(sig_reply, 4, "T%2.2x", signal_var);
		gdb_put_packet(connection, sig_reply, 3);
		gdb_connection->frontend_state = TARGET_HALTED;
		//rtos_update_threads( target );
	}
}

static int gdb_target_callback_event_handler(struct target *target,
		enum target_event event, void *priv)
{
	int retval;
	struct connection *connection = priv;

	target_handle_event(target, event);
	switch (event)
	{
		case TARGET_EVENT_GDB_HALT:
			gdb_frontend_halted(target, connection);
			break;
		case TARGET_EVENT_HALTED:
			target_call_event_callbacks(target, TARGET_EVENT_GDB_END);
			break;
		case TARGET_EVENT_GDB_FLASH_ERASE_START:
			target_handle_event(target, TARGET_EVENT_OLD_gdb_program_config);
			if ((retval = jtag_execute_queue()) != ERROR_OK)
			{
				return retval;
			}
			break;
		default:
			break;
	}

	return ERROR_OK;
}

static int gdb_new_connection(struct connection *connection)
{
	struct gdb_connection *gdb_connection = malloc(sizeof(struct gdb_connection));
	struct gdb_service *gdb_service = connection->service->priv;
	int retval;
	int initial_ack;

	connection->priv = gdb_connection;

	/* initialize gdb connection information */
	gdb_connection->buf_p = gdb_connection->buffer;
	gdb_connection->buf_cnt = 0;
	gdb_connection->ctrl_c = 0;
	gdb_connection->frontend_state = TARGET_HALTED;
	gdb_connection->vflash_image = NULL;
	gdb_connection->closed = 0;
	gdb_connection->busy = 0;
	gdb_connection->noack_mode = 0;
	gdb_connection->sync = true;
	gdb_connection->mem_write_error = false;

	/* send ACK to GDB for debug request */
	// mapu spu: Since we are the listener, we do not send ack.
	// Note: lldb does not check this ack.
	//gdb_write(connection, "+", 1);

	/* output goes through gdb connection */
	command_set_output_handler(connection->cmd_ctx, gdb_output, connection);

	/* we must remove all breakpoints registered to the target as a previous
	 * GDB session could leave dangling breakpoints if e.g. communication
	 * timed out.
	 */
	breakpoint_clear_target(gdb_service->target);
	watchpoint_clear_target(gdb_service->target);

	/* remove the initial ACK from the incoming buffer */
	if ((retval = gdb_get_char(connection, &initial_ack)) != ERROR_OK)
		return retval;

	/* FIX!!!??? would we actually ever receive a + here???
	 * Not observed.
	 * MaPU SPU: exactly
	 */
	if (initial_ack != '+')
		gdb_putback_char(connection, initial_ack);

	target_call_event_callbacks(gdb_service->target, TARGET_EVENT_GDB_ATTACH);

	/* Connect must fail if the memory map can't be set up correctly.
	 *
	 * This will cause an auto_probe to be invoked, which is either
	 * a no-op or it will fail when the target isn't ready(e.g. not halted).
	 */
/*	if (gdb_use_memory_map)
	{

		int i;
		for (i = 0; i < flash_get_bank_count(); i++)
		{
			struct flash_bank *p;
			retval = get_flash_bank_by_num(i, &p);
			if (retval != ERROR_OK)
			{
				LOG_ERROR("Connect failed. Consider setting up a gdb-attach event for the target to prepare target for GDB connect, or use 'gdb_memory_map disable'.");
				return retval;
			}
		}
	}
*/
	gdb_actual_connections++;
	LOG_DEBUG("New GDB Connection: %d, Target %s, state: %s",
	          gdb_actual_connections,
	          target_name(gdb_service->target),
	          target_state_name(gdb_service->target));

	/* DANGER! If we fail subsequently, we must remove this handler,
	 * otherwise we occasionally see crashes as the timer can invoke the
	 * callback fn.
	 *
	 * register callback to be informed about target events */
	target_register_event_callback(gdb_target_callback_event_handler, connection);

	return ERROR_OK;
}

static int gdb_connection_closed(struct connection *connection)
{
	struct gdb_service *gdb_service = connection->service->priv;
	struct gdb_connection *gdb_connection = connection->priv;

	/* we're done forwarding messages. Tear down callback before
	 * cleaning up connection.
	 */
	log_remove_callback(gdb_log_callback, connection);

	gdb_actual_connections--;
	LOG_DEBUG("GDB Close, Target: %s, state: %s, gdb_actual_connections=%d",
		  target_name(gdb_service->target),
		  target_state_name(gdb_service->target),
		  gdb_actual_connections);

	/* see if an image built with vFlash commands is left */
	if (gdb_connection->vflash_image)
	{
		// image_close(gdb_connection->vflash_image);
		free(gdb_connection->vflash_image);
		gdb_connection->vflash_image = NULL;
	}

	/* if this connection registered a debug-message receiver delete it */
	delete_debug_msg_receiver(connection->cmd_ctx, gdb_service->target);

	if (connection->priv)
	{
		free(connection->priv);
		connection->priv = NULL;
	}
	else
	{
		LOG_ERROR("BUG: connection->priv == NULL");
	}


	target_unregister_event_callback(gdb_target_callback_event_handler, connection);

	target_call_event_callbacks(gdb_service->target, TARGET_EVENT_GDB_END);

	target_call_event_callbacks(gdb_service->target, TARGET_EVENT_GDB_DETACH);

	return ERROR_OK;
}

static void gdb_send_error(struct connection *connection, uint8_t the_error)
{
	char err[4];
	snprintf(err, 4, "E%2.2X", the_error);
	gdb_put_packet(connection, err, 3);
}

static int gdb_last_signal_packet(struct connection *connection,
		struct target *target, char* packet, int packet_size)
{
  int sig = gdb_last_signal(target);
  char reply[20];
  int sz = sprintf(reply, "T%2.2xthread:%4.4x;", sig, target->tid);
  gdb_put_packet(connection, reply, sz);

	return ERROR_OK;
}

static int gdb_stop_reply_packet(struct connection *connection,
                                 struct target *target, char* packet, int packet_size)
{
  if(target->process_exited)
  {
    //  "WAA"
    //  "W" means the process exited and "AA" is the exit status.
    //
    //  "XAA"
    //  "X" means the process exited and "AA" is signal that caused the program
    //  to exit.
    // #define SIGQUIT   3 /* Quit (POSIX).  */
    char reply[8];
    int sz = sprintf(reply, "X%2.2x;", SIGQUIT);
    gdb_put_packet(connection, reply, sz);
  }
  else
    return gdb_last_signal_packet(connection, target, packet, packet_size);

  return ERROR_OK;
}

static int gdb_reg_pos(struct target *target, int pos, int len)
{
	if (target->endianness == TARGET_LITTLE_ENDIAN)
		return pos;
	else
		return len - 1 - pos;
}

/* Convert register to string of bytes. NB! The # of bits in the
 * register might be non-divisible by 8(a byte), in which
 * case an entire byte is shown.
 *
 * NB! the format on the wire is the target endianness
 *
 * The format of reg->value is little endian
 *
 */
static void gdb_str_to_target(struct target *target,
		char *tstr, struct reg *reg)
{
	int i;

	uint8_t *buf;
	int buf_len;
	buf = reg->value;
	buf_len = DIV_ROUND_UP(reg->desc->size, 8);

	for (i = 0; i < buf_len; i++)
	{
		int j = gdb_reg_pos(target, i, buf_len);
		tstr[i*2]   = DIGITS[(buf[j]>>4) & 0xf];
		tstr[i*2 + 1] = DIGITS[buf[j]&0xf];
	}
}

static int hextoint(int c)
{
	if (c>='0'&&c<='9')
	{
		return c-'0';
	}
	c = toupper(c);
	if (c>='A'&&c<='F')
	{
		return c-'A'+10;
	}
	LOG_ERROR("BUG: invalid register value %08x", c);
	return 0;
}

/* copy over in register buffer */
static void gdb_target_to_reg(struct target *target,
		char *tstr, int str_len, uint8_t *bin)
{
	if (str_len % 2)
	{
		LOG_ERROR("BUG: gdb value with uneven number of characters encountered");
		exit(-1);
	}

	int i;
	for (i = 0; i < str_len; i += 2)
	{
		uint8_t t = hextoint(tstr[i]) << 4;
		t |= hextoint(tstr[i + 1]);

		int j = gdb_reg_pos(target, i/2, str_len/2);
		bin[j] = t;
	}
}

static int gdb_get_registers_packet(struct connection *connection,
		struct target *target, char* packet, int packet_size)
{
	struct reg **reg_list;
	int reg_list_size;
	int retval;
	int reg_packet_size = 0;
	char *reg_packet;
	char *reg_packet_p;
	int i;

#ifdef _DEBUG_GDB_IO_
	LOG_DEBUG("-");
#endif

/*	if ( ( target->rtos != NULL ) &&
		 ( ERROR_FAIL != rtos_get_gdb_reg_list( connection, target, &reg_list, &reg_list_size) ) )
	{
		return ERROR_OK;
	}
*/
	if ((retval = target_get_gdb_reg_list(target, &reg_list, &reg_list_size)) != ERROR_OK)
	{
		return gdb_error(connection, retval);
	}

	for (i = 0; i < reg_list_size; i++)
	{
		reg_packet_size += reg_list[i]->desc->size;
	}

	reg_packet = malloc(DIV_ROUND_UP(reg_packet_size, 8) * 2);
	reg_packet_p = reg_packet;

	for (i = 0; i < reg_list_size; i++)
	{
		if (!*reg_list[i]->valid)
			reg_list[i]->desc->type->get(reg_list[i]);
		gdb_str_to_target(target, reg_packet_p, reg_list[i]);
		reg_packet_p += DIV_ROUND_UP(reg_list[i]->desc->size, 8) * 2;
	}

#ifdef _DEBUG_GDB_IO_
	{
		char *reg_packet_p;
		reg_packet_p = strndup(reg_packet, DIV_ROUND_UP(reg_packet_size, 8) * 2);
		LOG_DEBUG("reg_packet: %s", reg_packet_p);
		free(reg_packet_p);
	}
#endif

	gdb_put_packet(connection, reg_packet, DIV_ROUND_UP(reg_packet_size, 8) * 2);
	free(reg_packet);

	free(reg_list);

	return ERROR_OK;
}

static int gdb_set_registers_packet(struct connection *connection,
		struct target *target, char *packet, int packet_size)
{
	int i;
	struct reg **reg_list;
	int reg_list_size;
	int retval;
	char *packet_p;

#ifdef _DEBUG_GDB_IO_
	LOG_DEBUG("-");
#endif

	/* skip command character */
	packet++;
	packet_size--;

	if (packet_size % 2)
	{
		LOG_WARNING("GDB set_registers packet with uneven characters received, dropping connection");
		return ERROR_SERVER_REMOTE_CLOSED;
	}

	if ((retval = target_get_gdb_reg_list(target, &reg_list, &reg_list_size)) != ERROR_OK)
	{
		return gdb_error(connection, retval);
	}

	packet_p = packet;
	for (i = 0; i < reg_list_size; i++)
	{
		uint8_t *bin_buf;
		int chars = (DIV_ROUND_UP(reg_list[i]->desc->size, 8) * 2);

		if (packet_p + chars > packet + packet_size)
		{
			LOG_ERROR("BUG: register packet is too small for registers");
		}

		bin_buf = malloc(DIV_ROUND_UP(reg_list[i]->desc->size, 8));
		gdb_target_to_reg(target, packet_p, chars, bin_buf);

		reg_list[i]->desc->type->set(reg_list[i], bin_buf);

		/* advance packet pointer */
		packet_p += chars;


		free(bin_buf);
	}

	/* free struct reg *reg_list[] array allocated by get_gdb_reg_list */
	free(reg_list);

	gdb_put_packet(connection, "OK", 2);

	return ERROR_OK;
}

static int gdb_get_register_packet(struct connection *connection,
		struct target *target, char *packet, int packet_size)
{
	char *reg_packet;
	int reg_num = strtoul(packet + 1, NULL, 16);
	struct reg **reg_list;
	int reg_list_size;
	int retval;

#ifdef _DEBUG_GDB_IO_
	LOG_DEBUG("-");
#endif

	if ((retval = target_get_gdb_reg_list(target, &reg_list, &reg_list_size)) != ERROR_OK)
	{
		return gdb_error(connection, retval);
	}

	if (reg_list_size <= reg_num)
	{
		LOG_ERROR("gdb requested a non-existing register");
		exit(-1);
	}

	struct reg * r = (*reg_list + reg_num);

	if (!*r->valid) {
	  r->desc->type->get(r);
	}

	reg_packet = malloc(DIV_ROUND_UP(r->desc->size, 8) * 2);

	gdb_str_to_target(target, reg_packet, r);

	gdb_put_packet(connection, reg_packet, DIV_ROUND_UP(r->desc->size, 8) * 2);

	// since we do not create a new register list for mapu spu, we do not free it.
	//free(reg_list);

	free(reg_packet);

	return ERROR_OK;
}

static int gdb_set_register_packet(struct connection *connection,
		struct target *target, char *packet, int packet_size)
{
	char *separator;
	uint8_t *bin_buf;
	int reg_num = strtoul(packet + 1, &separator, 16);
	struct reg **reg_list;
	int reg_list_size;
	int retval;

	LOG_DEBUG("-");

	if ((retval = target_get_gdb_reg_list(target, &reg_list, &reg_list_size)) != ERROR_OK)
	{
		return gdb_error(connection, retval);
	}

	if (reg_list_size < reg_num)
	{
		LOG_ERROR("gdb requested a non-existing register");
		return ERROR_SERVER_REMOTE_CLOSED;
	}

	if (*separator != '=')
	{
		LOG_ERROR("GDB 'set register packet', but no '=' following the register number");
		return ERROR_SERVER_REMOTE_CLOSED;
	}

	struct reg * r = (*reg_list + reg_num);

	/* convert from GDB-string (target-endian) to hex-string (big-endian) */
	bin_buf = malloc(DIV_ROUND_UP(r->desc->size, 8));
	int chars = (DIV_ROUND_UP(r->desc->size, 8) * 2);

	/* fix!!! add some sanity checks on packet size here */

	gdb_target_to_reg(target, separator + 1, chars, bin_buf);

	r->desc->type->set(r, bin_buf);

	gdb_put_packet(connection, "OK", 2);

	free(bin_buf);
	free(reg_list);

	return ERROR_OK;
}

/* No attempt is made to translate the "retval" to
 * GDB speak. This has to be done at the calling
 * site as no mapping really exists.
 */
static int gdb_error(struct connection *connection, int retval)
{
	LOG_DEBUG("Reporting %i to GDB as generic error", retval);
	gdb_send_error(connection, EFAULT);
	return ERROR_OK;
}

/* We don't have to worry about the default 2 second timeout for GDB packets,
 * because GDB breaks up large memory reads into smaller reads.
 *
 * 8191 bytes by the looks of it. Why 8191 bytes instead of 8192?????
 */
static int gdb_read_memory_packet(struct connection *connection,
		struct target *target, char *packet, int packet_size)
{
	char *separator;
	uint32_t addr = 0;
	uint32_t length = 0;

	uint8_t *buffer;
	char *hex_buffer;

	int retval = ERROR_OK;

	/* skip command character */
	packet++;

	addr = strtoul(packet, &separator, 16);

	if (*separator != ',')
	{
		LOG_ERROR("incomplete read memory packet received, dropping connection");
		return ERROR_SERVER_REMOTE_CLOSED;
	}

	length = strtoul(separator + 1, NULL, 16);

	buffer = malloc(length);

	LOG_DEBUG("addr: 0x%8.8" PRIx32 ", length: 0x%8.8" PRIx32 "", addr, length);

	retval = target_read_buffer(target, addr, length, buffer);

	if ((retval != ERROR_OK)&&!gdb_report_data_abort)
	{
		/* TODO : Here we have to lie and send back all zero's lest stack traces won't work.
		 * At some point this might be fixed in GDB, in which case this code can be removed.
		 *
		 * OpenOCD developers are acutely aware of this problem, but there is nothing
		 * gained by involving the user in this problem that hopefully will get resolved
		 * eventually
		 *
		 * http://sourceware.org/cgi-bin/gnatsweb.pl?cmd = view%20audit-trail&database = gdb&pr = 2395
		 *
		 * For now, the default is to fix up things to make current GDB versions work.
		 * This can be overwritten using the gdb_report_data_abort <'enable'|'disable'> command.
		 */
		memset(buffer, 0, length);
		retval = ERROR_OK;
	}

	if (retval == ERROR_OK)
	{
		hex_buffer = malloc(length * 2 + 1);

		uint32_t i;
		for (i = 0; i < length; i++)
		{
			uint8_t t = buffer[i];
			hex_buffer[2 * i] = DIGITS[(t >> 4) & 0xf];
			hex_buffer[2 * i + 1] = DIGITS[t & 0xf];
		}

		gdb_put_packet(connection, hex_buffer, length * 2);

		free(hex_buffer);
	}
	else
	{
		retval = gdb_error(connection, retval);
	}

	free(buffer);

	return retval;
}

static int gdb_write_memory_packet(struct connection *connection,
		struct target *target, char *packet, int packet_size)
{
	char *separator;
	uint32_t addr = 0;
	uint32_t length = 0;

	uint8_t *buffer;

	uint32_t i;
	int retval;

	/* skip command character */
	packet++;

	addr = strtoul(packet, &separator, 16);

	if (*separator != ',')
	{
		LOG_ERROR("incomplete write memory packet received, dropping connection");
		return ERROR_SERVER_REMOTE_CLOSED;
	}

	length = strtoul(separator + 1, &separator, 16);

	if (*(separator++) != ':')
	{
		LOG_ERROR("incomplete write memory packet received, dropping connection");
		return ERROR_SERVER_REMOTE_CLOSED;
	}

	buffer = malloc(length);

	LOG_DEBUG("addr: 0x%8.8" PRIx32 ", length: 0x%8.8" PRIx32 "", addr, length);

	for (i = 0; i < length; i++)
	{
		uint32_t tmp;
		sscanf(separator + 2*i, "%2" SCNx32 , &tmp);
		buffer[i] = tmp;
	}

	retval = target_write_buffer(target, addr, length, buffer);

	if (retval == ERROR_OK)
	{
		gdb_put_packet(connection, "OK", 2);
	}
	else
	{
		retval = gdb_error(connection, retval);
	}

	free(buffer);

	return retval;
}

static int gdb_write_memory_binary_packet(struct connection *connection,
		struct target *target, char *packet, int packet_size)
{
	char *separator;
	uint32_t addr = 0;
	uint32_t length = 0;

	int retval = ERROR_OK;

	/* skip command character */
	packet++;

	addr = strtoul(packet, &separator, 16);

	if (*separator != ',')
	{
		LOG_ERROR("incomplete write memory binary packet received, dropping connection");
		return ERROR_SERVER_REMOTE_CLOSED;
	}

	length = strtoul(separator + 1, &separator, 16);

	if (*(separator++) != ':')
	{
		LOG_ERROR("incomplete write memory binary packet received, dropping connection");
		return ERROR_SERVER_REMOTE_CLOSED;
	}

	struct gdb_connection *gdb_connection = connection->priv;

	if (gdb_connection->mem_write_error)
	{
		retval = ERROR_FAIL;
		/* now that we have reported the memory write error, we can clear the condition */
		gdb_connection->mem_write_error = false;
	}

	/* By replying the packet *immediately* GDB will send us a new packet
	 * while we write the last one to the target.
	 */
	if (retval == ERROR_OK)
	{
		gdb_put_packet(connection, "OK", 2);
	}
	else
	{
		if ((retval = gdb_error(connection, retval)) != ERROR_OK)
			return retval;
	}

	if (length)
	{
		LOG_DEBUG("addr: 0x%8.8" PRIx32 ", length: 0x%8.8" PRIx32 "", addr, length);

		retval = target_write_buffer(target, addr, length, (uint8_t*)separator);
		if (retval != ERROR_OK)
		{
			gdb_connection->mem_write_error = true;
		}
	}

	return ERROR_OK;
}

static int gdb_step_continue_packet(struct connection *connection,
		struct target *target, char *packet, int packet_size)
{
	int current = 0;
	uint32_t address = 0x0;
	int retval = ERROR_OK;

	LOG_DEBUG("-");

	if (packet_size > 1)
	{
		packet[packet_size] = 0;
		address = strtoul(packet + 1, NULL, 16);
	}
	else
	{
		current = 1;
	}

	if (packet[0] == 'c')
	{
		LOG_DEBUG("continue");
		target_handle_event(target, TARGET_EVENT_OLD_pre_resume);
		/* resume at current address, don't handle breakpoints, not debugging */
		retval = target_resume(target, current, address, 0, 0);
	}
	else if (packet[0] == 's')
	{
		LOG_DEBUG("step");
		/* step at current or address, don't handle breakpoints */
		retval = target_step(target, current, address, 0);
	}
	return retval;
}

static int gdb_breakpoint_watchpoint_packet(struct connection *connection,
		struct target *target, char *packet, int packet_size)
{
	int type;
	enum breakpoint_type bp_type = BKPT_SOFT /* dummy init to avoid warning */;
	enum watchpoint_rw wp_type = WPT_READ /* dummy init to avoid warning */;
	uint32_t address;
	uint32_t size;
	char *separator;
	int retval;

	LOG_DEBUG("-");

	type = strtoul(packet + 1, &separator, 16);

	if (type == 0)	/* memory breakpoint */
		bp_type = BKPT_SOFT;
	else if (type == 1) /* hardware breakpoint */
		bp_type = BKPT_HARD;
	else if (type == 2) /* write watchpoint */
		wp_type = WPT_WRITE;
	else if (type == 3) /* read watchpoint */
		wp_type = WPT_READ;
	else if (type == 4) /* access watchpoint */
		wp_type = WPT_ACCESS;
	else
	{
		LOG_ERROR("invalid gdb watch/breakpoint type(%d), dropping connection", type);
		return ERROR_SERVER_REMOTE_CLOSED;
	}


	if (gdb_breakpoint_override && ((bp_type == BKPT_SOFT)||(bp_type == BKPT_HARD)))
	{
		bp_type = gdb_breakpoint_override_type;
	}

	if (*separator != ',')
	{
		LOG_ERROR("incomplete breakpoint/watchpoint packet received, dropping connection");
		return ERROR_SERVER_REMOTE_CLOSED;
	}

	address = strtoul(separator + 1, &separator, 16);

	if (*separator != ',')
	{
		LOG_ERROR("incomplete breakpoint/watchpoint packet received, dropping connection");
		return ERROR_SERVER_REMOTE_CLOSED;
	}

	size = strtoul(separator + 1, &separator, 16);

	switch (type)
	{
		case 0:
		case 1:
			if (packet[0] == 'Z')
			{
				if ((retval = breakpoint_add(target, address, size, bp_type)) != ERROR_OK)
				{
					if ((retval = gdb_error(connection, retval)) != ERROR_OK)
						return retval;
				}
				else
				{
					gdb_put_packet(connection, "OK", 2);
				}
			}
			else
			{
				breakpoint_remove(target, address);
				gdb_put_packet(connection, "OK", 2);
			}
			break;
		case 2:
		case 3:
		case 4:
		{
			if (packet[0] == 'Z')
			{
				if ((retval = watchpoint_add(target, MODE_MEM, address, size, wp_type, 0, 0xffffffffu)) != ERROR_OK)
				{
					if ((retval = gdb_error(connection, retval)) != ERROR_OK)
						return retval;
				}
				else
				{
					gdb_put_packet(connection, "OK", 2);
				}
			}
			else
			{
				watchpoint_remove(target, address);
				gdb_put_packet(connection, "OK", 2);
			}
			break;
		}
		default:
			break;
	}

	return ERROR_OK;
}

/*static int compare_bank (const void * a, const void * b)
{
	struct flash_bank *b1, *b2;
	b1=*((struct flash_bank **)a);
	b2=*((struct flash_bank **)b);

	if (b1->base == b2->base)
	{
		return 0;
	} else if (b1->base > b2->base)
	{
		return 1;
	} else
	{
		return -1;
	}

	return 0;
}

static int gdb_memory_map(struct connection *connection,
		struct target *target, char *packet, int packet_size)
{

	return ERROR_OK;
}
*/

/* TO ADD: query packet deleted */

static int gdb_v_packet(struct connection *connection,
		struct target *target, char *packet, int packet_size)
{
  // struct gdb_connection *gdb_connection = connection->priv;
	struct gdb_service *gdb_service = connection->service->priv;
	const char* str;
	int result;
	size_t length;

	/* if flash programming disabled - send a empty reply */
	if (gdb_flash_program == 0)
	{
		gdb_put_packet(connection, "", 0);
		return ERROR_OK;
	}

	if (strstr(packet, "vFlashErase:"))
	{
		unsigned long addr;
		unsigned long length;

		char *parse = packet + 12;
		if (*parse == '\0')
		{
			LOG_ERROR("incomplete vFlashErase packet received, dropping connection");
			return ERROR_SERVER_REMOTE_CLOSED;
		}

		addr = strtoul(parse, &parse, 16);

		if (*(parse++) != ',' || *parse == '\0')
		{
			LOG_ERROR("incomplete vFlashErase packet received, dropping connection");
			return ERROR_SERVER_REMOTE_CLOSED;
		}

		length = strtoul(parse, &parse, 16);

		if (*parse != '\0')
		{
			LOG_ERROR("incomplete vFlashErase packet received, dropping connection");
			return ERROR_SERVER_REMOTE_CLOSED;
		}

		/* assume all sectors need erasing - stops any problems
		 * when flash_write is called multiple times */
		//flash_set_dirty();

		/* perform any target specific operations before the erase */
		target_call_event_callbacks(gdb_service->target,
				TARGET_EVENT_GDB_FLASH_ERASE_START);

		/* vFlashErase:addr,length messages require region start and
		 * end to be "block" aligned ... if padding is ever needed,
		 * GDB will have become dangerously confused.
		 */
    /*result = flash_erase_address_range(gdb_service->target, false, addr, length);*/

		/* perform any target specific operations after the erase */
		target_call_event_callbacks(gdb_service->target, TARGET_EVENT_GDB_FLASH_ERASE_END);

		/* perform erase */
		if (0) //result != ERROR_OK)
		{
			/* GDB doesn't evaluate the actual error number returned,
			 * treat a failed erase as an I/O error
			 */
			gdb_send_error(connection, EIO);
			LOG_ERROR("flash_erase returned %i", result);
		}
		else
			gdb_put_packet(connection, "OK", 2);

		return ERROR_OK;
	}

	else if (strstr(packet, "vFlashWrite:"))
	{
	  // int retval;
		unsigned long addr;
		unsigned long length;
		char *parse = packet + 12;

		if (*parse == '\0')
		{
			LOG_ERROR("incomplete vFlashErase packet received, dropping connection");
			return ERROR_SERVER_REMOTE_CLOSED;
		}
		addr = strtoul(parse, &parse, 16);
		if (*(parse++) != ':')
		{
			LOG_ERROR("incomplete vFlashErase packet received, dropping connection");
			return ERROR_SERVER_REMOTE_CLOSED;
		}
		length = packet_size - (parse - packet);

		/* create a new image if there isn't already one */
		/*if (gdb_connection->vflash_image == NULL)
		{
			gdb_connection->vflash_image = malloc(sizeof(struct image));
			image_open(gdb_connection->vflash_image, "", "build");
		}*/

		/* create new section with content from packet buffer */
		/*if ((retval = image_add_section(gdb_connection->vflash_image, addr, length, 0x0, (uint8_t*)parse)) != ERROR_OK)
		{
			return retval;
		}*/
		gdb_put_packet(connection, "OK", 2);

		return ERROR_OK;
	}

	else if(str = "vCont?", strncmp(packet, str, strlen(str)) == 0) {
    char reply[] = "vCont;c;s";
    gdb_put_packet(connection, reply, sizeof(reply)-1); // tailing NULL
    return ERROR_OK;
  }

  else if(str = "vCont;s:", strncmp(packet, str, length = strlen(str)) == 0) {
    unsigned long tid = strtoul(packet + length, NULL, 16);
    if(tid != target->tid)
    {
      LOG_ERROR("thread ID specified by remote debugger does not match with current target.");
      return ERROR_FAIL;
    }

    /* step at current or address, don't handle breakpoints */
    int retval = target_step(target, 1/*current*/, 0/*address*/, 0/*don't handle breakpoints*/);
    //target_poll(target); // need not to poll here.
    if(target->state != TARGET_HALTED)
    {
      LOG_ERROR("thread %d of process %d can not be halted", target->pid, target->tid);
      gdb_send_error(connection, EPERM);
      return ERROR_FAIL;
    }

    gdb_stop_reply_packet(connection, target, packet, packet_size);
    return ERROR_OK;
  }

  else if(str = "vAttach;", strncmp(packet, str, length = strlen(str)) == 0) {
    // if target is halted, send a reply packet. otherwise is an error.
    uint32_t pid = strtoul(packet+length, NULL, 16);

    if(pid != target->pid)
    {
      LOG_ERROR("PID specified by remote debugger does not match with current target.");
      gdb_send_error(connection, EPERM);
      return ERROR_FAIL;
    }

    if(target->state == TARGET_HALTED)
    {
      gdb_stop_reply_packet(connection, target, packet, packet_size);
    }
    else {
      target_halt(target);
      target_poll(target);
      if(target->state != TARGET_HALTED)
      {
        LOG_ERROR("process %d can not be halted", pid);
        gdb_send_error(connection, EPERM);
        return ERROR_FAIL;
      }

      gdb_stop_reply_packet(connection, target, packet, packet_size);
    }

    return ERROR_OK;
  }

	/* process the flashing buffer. No need to erase as GDB
	 * always issues a vFlashErase first. */

	/*if (!strcmp(packet, "vFlashDone"))
	{
		uint32_t written;


		target_call_event_callbacks(gdb_service->target, TARGET_EVENT_GDB_FLASH_WRITE_START);
		result = flash_write(gdb_service->target, gdb_connection->vflash_image, &written, 0);
		target_call_event_callbacks(gdb_service->target, TARGET_EVENT_GDB_FLASH_WRITE_END);
		if (result != ERROR_OK)
		{
			if (result == ERROR_FLASH_DST_OUT_OF_BANK)
				gdb_put_packet(connection, "E.memtype", 9);
			else
				gdb_send_error(connection, EIO);
			}
		else
		{
			LOG_DEBUG("wrote %u bytes from vFlash image to flash", (unsigned)written);
			gdb_put_packet(connection, "OK", 2);
		}

		image_close(gdb_connection->vflash_image);
		free(gdb_connection->vflash_image);
		gdb_connection->vflash_image = NULL;

		return ERROR_OK;
	}*/

	// v-packet handling end, reply null
	gdb_put_packet(connection, "", 0);
	return ERROR_OK;
}

static int gdb_detach(struct connection *connection, struct target *target)
{
	struct gdb_service *gdb_service = connection->service->priv;

	target_call_event_callbacks(gdb_service->target,
			TARGET_EVENT_GDB_DETACH);

	return gdb_put_packet(connection, "OK", 2);
}

static void gdb_log_callback(void *priv, const char *file, unsigned line,
		const char *function, const char *string)
{
	struct connection *connection = priv;
	struct gdb_connection *gdb_con = connection->priv;

	if (gdb_con->busy)
	{
		/* do not reply this using the O packet */
		return;
	}

	// MaPU SPU:
	// this callback will result in hazard for log
	//gdb_output_con(connection, string);
}

/* print out a string and allocate more space as needed,
 * mainly used for XML at this point
 */
static void xml_printf(int *retval, char **xml, int *pos, int *size,
    const char *fmt, ...)
{
  if (*retval != ERROR_OK)
    return;
  int first = 1;

  for (;; ) {
    if ((*xml == NULL) || (!first)) {
      /* start by 0 to exercise all the code paths.
       * Need minimum 2 bytes to fit 1 char and 0 terminator. */

      *size = *size * 2 + 2;
      char *t = *xml;
      *xml = realloc(*xml, *size);
      if (*xml == NULL) {
        if (t)
          free(t);
        *retval = ERROR_SERVER_REMOTE_CLOSED;
        return;
      }
    }

    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = vsnprintf(*xml + *pos, *size - *pos, fmt, ap);
    va_end(ap);
    if ((ret > 0) && ((ret + 1) < *size - *pos)) {
      *pos += ret;
      return;
    }
    /* there was just enough or not enough space, allocate more. */
    first = 0;
  }
}

static int decode_xfer_read(char *buf, char **annex, int *ofs, unsigned int *len)
{
  char *separator;

  /* Extract and NUL-terminate the annex. */
  *annex = buf;
  while (*buf && *buf != ':')
    buf++;
  if (*buf == '\0')
    return -1;
  *buf++ = 0;

  /* After the read marker and annex, qXfer looks like a
   * traditional 'm' packet. */

  *ofs = strtoul(buf, &separator, 16);

  if (*separator != ',')
    return -1;

  *len = strtoul(separator + 1, NULL, 16);

  return 0;
}

static int gdb_query_packet(struct connection *connection,
    char *packet, int packet_size)
{
  struct command_context *cmd_ctx = connection->cmd_ctx;
  struct gdb_connection *gdb_connection = connection->priv;
  struct target *target = get_target_from_connection(connection);
  struct target_type *target_type = target->type;
  const char* str;
  int retval;
  size_t length;

  if (strncmp(packet, "qRcmd,", 6) == 0) {
    if (packet_size > 6) {
      char *cmd;
      cmd = malloc((packet_size - 6) / 2 + 1);
      int length = unhexify(cmd, packet + 6, (packet_size - 6) / 2);
      cmd[length] = 0;

      /* We want to print all debug output to GDB connection */
      log_add_callback(gdb_log_callback, connection);

      target_call_timer_callbacks_now();
      /* some commands need to know the GDB connection, make note of current
       * GDB connection. */
      current_gdb_connection = gdb_connection;
      command_run_line(cmd_ctx, cmd);
      current_gdb_connection = NULL;
      target_call_timer_callbacks_now();
      log_remove_callback(gdb_log_callback, connection);
      free(cmd);
    }

    gdb_put_packet(connection, "OK", 2);
    return ERROR_OK;
  }

  else if (strncmp(packet, "qLaunchSuccess", 14) == 0) {
    // check whether process is ready: running or halted, not reset.
    //target = get_current_target(cmd_ctx);
    retval = target_poll(target);
    if(target->state == TARGET_HALTED) {
      gdb_put_packet(connection, "OK", 2);
      target->debug_reason = DBG_REASON_DBGRQ;
    }
    else if(target->state == TARGET_RUNNING) {
      gdb_send_error(connection, TARGET_RUNNING);
      target->debug_reason = DBG_REASON_NOTHALTED;
    }
    else {
      gdb_send_error(connection, TARGET_UNKNOWN);
      target->debug_reason = DBG_REASON_UNDEFINED;
    }

    target->pid = target->gdb_port;
    target->tid = 1;

    return ERROR_OK;
  }

  else if (str = "QThreadSuffixSupported", strncmp(packet, str, strlen(str)) == 0) {
    gdb_put_packet(connection, "OK", 2);
    return ERROR_OK;
  }

  else if (str = "QListThreadsInStopReply", strncmp(packet, str, strlen(str)) == 0) {
    gdb_put_packet(connection, "OK", 2);
    return ERROR_OK;
  }

  else if (str = "qMemoryRegionInfo:", strncmp(packet, str, length = strlen(str)) == 0) {
    uint32_t addr = strtoul(packet+length, NULL, 16);
    if(addr < 0x20000UL)
    {
      uint32_t addr1 = addr & (~3UL);
      char reply[65];
      int sz = sprintf(reply, "start:%x;size:%x;permissions:rwx", addr1, 0x20000-addr1);
      gdb_put_packet(connection, reply, sz);
    }
    else
    {
      char reply[] = "error:invalid mspu memory address";
      gdb_put_packet(connection, reply, sizeof(reply));
    }

    return ERROR_OK;
  }

  else if (str = "qThreadStopInfo", strncmp(packet, str, strlen(str)) == 0) {
    gdb_stop_reply_packet(connection, target, packet, packet_size);
    return ERROR_OK;
  }

  else if (str = "qRegisterInfo", strncmp(packet, str, length = strlen(str)) == 0) {
    int reg_num = strtoul(packet + length, NULL, 16);
    struct reg **reg_list_head;
    int reg_list_size;
    int retval;
    if ((retval = target_get_gdb_reg_list(target, &reg_list_head, &reg_list_size)) != ERROR_OK)
    {
      return gdb_error(connection, retval);
    }

    if(reg_num>=reg_list_size)
    {
      gdb_put_packet(connection, "", 0);
      return ERROR_OK;
    }

    struct reg * reg = (*reg_list_head) + reg_num;
    struct mapu_spu_reg_info *reg_info = reg->desc->arch_info;
    struct mapu_spu_reg_type *type = reg_info->type;
    char reg_desc[512];
    int sz = 0;

    // register info:
    // name:r13;bitsize:64;offset:104;encoding:uint;format:hex;set:General Purpose Registers;gcc:13;dwarf:13;
    sz += sprintf(reg_desc+sz, "name:%s;", reg->desc->name);

    if(reg_info->alt_name) {
      sz += sprintf(reg_desc+sz, "alt-name:%s;", reg_info->alt_name);
      sz += sprintf(reg_desc+sz, "generic:%s;", reg_info->alt_name);
    }

    sz += sprintf(reg_desc+sz, "bitsize:%d;", reg->desc->size);
    sz += sprintf(reg_desc+sz, "offset:%d;", reg->desc->offset);
    sz += sprintf(reg_desc+sz, "encoding:%s;", type->encoding_name);
    sz += sprintf(reg_desc+sz, "format:%s;", type->format_name);
    sz += sprintf(reg_desc+sz, "set:%s;", type->set_name);
    sz += sprintf(reg_desc+sz, "gcc:%d;", reg_info->no);
    sz += sprintf(reg_desc+sz, "dwarf:%d;", reg_info->no);

    if(reg_info->num_super_regs || reg_info->num_sub_regs)
    {
      sz += sprintf(reg_desc+sz, "container-regs:");

      if(reg_info->num_super_regs > 0)
      {
        sz += sprintf(reg_desc+sz, "%x", reg_info->super_regs[0]);
        for(int k = 1; k<reg_info->num_super_regs; k++)
          sz += sprintf(reg_desc+sz, ",%x", reg_info->super_regs[k]);

        for(int k = 0; k<reg_info->num_sub_regs; k++)
          sz += sprintf(reg_desc+sz, ",%x", reg_info->sub_regs[k]);
      }
      else if(reg_info->num_sub_regs > 0)
      {
        sz += sprintf(reg_desc+sz, "%x", reg_info->sub_regs[0]);
        for(int k = 1; k<reg_info->num_sub_regs; k++)
          sz += sprintf(reg_desc+sz, ",%x", reg_info->sub_regs[k]);
      }

      sz += sprintf(reg_desc+sz, ";");

      ////////////////////////////////////////////////////////////
      sz += sprintf(reg_desc+sz, "invalidate-regs:");

      if(reg_info->num_super_regs > 0)
      {
        sz += sprintf(reg_desc+sz, "%x", reg_info->super_regs[0]);
        for(int k = 1; k<reg_info->num_super_regs; k++)
          sz += sprintf(reg_desc+sz, ",%x", reg_info->super_regs[k]);

        for(int k = 0; k<reg_info->num_sub_regs; k++)
          sz += sprintf(reg_desc+sz, ",%x", reg_info->sub_regs[k]);
      }
      else if(reg_info->num_sub_regs > 0)
      {
        sz += sprintf(reg_desc+sz, "%x", reg_info->sub_regs[0]);
        for(int k = 1; k<reg_info->num_sub_regs; k++)
          sz += sprintf(reg_desc+sz, ",%x", reg_info->sub_regs[k]);
      }

      sz += sprintf(reg_desc+sz, ";");
    }

    gdb_put_packet(connection, reg_desc, sz);
    return ERROR_OK;
  }

  else if (str = "qVAttachOrWaitSupported", strncmp(packet, str, strlen(str)) == 0) {
    gdb_put_packet(connection, "OK", 2);
    return ERROR_OK;
  }

  else if (strncmp(packet, "qC", 2) == 0) {
    //target = get_current_target(cmd_ctx);

    // remote debuger asks for process ID
    target_type = target->type;
    target_type->soft_reset_halt(target);
    target->debug_reason = DBG_REASON_DBGRQ;

    if(target->pid > 0) {
      char buf[2+8+1]; // "QC" + pid + '\0'
      int sz = snprintf(buf, 11, "QC%x", target->pid);
      gdb_put_packet(connection, buf, sz);
      return ERROR_OK;
    }
    else
    {
      gdb_send_error(connection, ESRCH);
      return ERROR_FAIL;
    }
  }

  else if(str = "QLaunchArch:mspu", strncmp(packet, str, strlen(str)) == 0) {
    // this is where we prepare a unused core to run a spu program.
    target = all_targets;
    while(target && target->gdb_connected)
    {
      target = target->next;
    }

    // find a target not connected to lldb
    if(target)
    {
      target_type = target->type;
      target_type->soft_reset_halt(target);
      target->debug_reason = DBG_REASON_DBGRQ;

      // set this target as *current target*
      cmd_ctx->current_target = target->target_number;
      LOG_INFO("%s %s is selected as the active target due to remote lldb connection",
               target_type_name(target), target_name(target));

      gdb_put_packet(connection, "OK", 2);
      return ERROR_OK;
    }
    else
    {
      // we do not have unused core for a new lldb connection
      gdb_send_error(connection, EBUSY);
      return ERROR_FAIL;
    }
  }

  else if(str =  "qLaunchGDBServer:port:0;", strncmp(packet, str, strlen(str)) == 0) {
    // reply with "port:%u;" since we do not start a new debug server process
    //target = get_current_target(cmd_ctx);
    char reply[6 + 4 + 1];
    int sz = snprintf(reply, sizeof(reply), "port:%u;", target->gdb_port);
    gdb_put_packet(connection, reply, sz);
    target->gdb_connected = true;
    return ERROR_OK;
  }

  else if (strncmp(packet, "qCRC:", 5) == 0) {
    if (packet_size > 5) {
      int retval;
      char gdb_reply[10];
      char *separator;
      uint32_t checksum;
      uint32_t addr = 0;

      /* skip command character */
      packet += 5;

      addr = strtoul(packet, &separator, 16);

      if (*separator != ',') {
        LOG_ERROR("incomplete read memory packet received, dropping connection");
        return ERROR_SERVER_REMOTE_CLOSED;
      }

      length = strtoul(separator + 1, NULL, 16);

      retval = target_checksum_memory(target, addr, length, &checksum);

      if (retval == ERROR_OK) {
        snprintf(gdb_reply, 10, "C%8.8" PRIx32 "", checksum);
        gdb_put_packet(connection, gdb_reply, 9);
      } else {
        retval = gdb_error(connection, retval);
        if (retval != ERROR_OK)
          return retval;
      }

      return ERROR_OK;
    }
  } else if (strncmp(packet, "qSupported", 10) == 0) {
    /* we currently support packet size and qXfer:memory-map:read (if enabled)
     * disable qXfer:features:read for the moment */

    /*int retval = ERROR_OK;
    char *buffer = NULL;
    int pos = 0;
    int size = 0;

    xml_printf(&retval,
      &buffer,
      &pos,
      &size,
      "PacketSize=%x;qXfer:memory-map:read%c;qXfer:features:read-;QStartNoAckMode+",
      (GDB_BUFFER_SIZE - 1),
      ((gdb_use_memory_map == 1) && (flash_get_bank_count() > 0)) ? '+' : '-');

    if (retval != ERROR_OK) {
      gdb_send_error(connection, 01);
      return ERROR_OK;
    }

    gdb_put_packet(connection, buffer, strlen(buffer));
    free(buffer);*/

    return ERROR_OK;
  }

  /*else if ((strncmp(packet, "qXfer:memory-map:read::", 23) == 0) && (flash_get_bank_count() > 0))
    return gdb_memory_map(connection, packet, packet_size);*/

  else if (strncmp(packet, "qXfer:features:read:", 20) == 0) {
    char *xml = NULL;
    int size = 0;
    int pos = 0;
    int retval = ERROR_OK;

    int offset;
    unsigned int length;
    char *annex;

    /* skip command character */
    packet += 20;

    if (decode_xfer_read(packet, &annex, &offset, &length) < 0) {
      gdb_send_error(connection, 01);
      return ERROR_OK;
    }

    if (strcmp(annex, "target.xml") != 0) {
      gdb_send_error(connection, 01);
      return ERROR_OK;
    }

    xml_printf(&retval,
      &xml,
      &pos,
      &size, \
      "l < target version=\"1.0\">\n < architecture > arm</architecture>\n</target>\n");

    if (retval != ERROR_OK) {
      gdb_error(connection, retval);
      return retval;
    }

    gdb_put_packet(connection, xml, strlen(xml));

    free(xml);
    return ERROR_OK;
  }

  else if (strncmp(packet, "QStartNoAckMode", 15) == 0) {
    // we always expect an ack for this "OK" reply.
    gdb_put_packet(connection, "OK", 2);
    // and then we do not expect '+' acks.
    gdb_connection->noack_mode = 1;
    return ERROR_OK;
  }

  // add by kulw at 2013.12.10
  else if (strncmp(packet, "qHostInfo", 9) == 0) {
    char hostname[260];
    hostname[sizeof(hostname) - 1] = '\0';

    if (gethostname (hostname, sizeof(hostname) - 1) == 0)
    {
      char *s = NULL;
      const char prefix[] = "hostname:";
      char strHostinfo[520];

      struct hostent* h = gethostbyname (hostname);
      if (h)
          s = (h->h_name);
      else
          s = (hostname);

      memcpy(strHostinfo, prefix, sizeof(prefix)-1);
      size_t sz = hexify(strHostinfo + sizeof(prefix)-1, s, 0, sizeof(strHostinfo));
      sz += (sizeof(prefix)-1);
      strHostinfo[sz++] = ';';

      // since jtag is rather slow, we have to designate a timeout large enough.
      sz += sprintf(strHostinfo+sz, "default_packet_timeout:%d;", 100/* seconds */);

      strHostinfo[sz] = '\0';
      gdb_put_packet(connection, strHostinfo, sz);
      return ERROR_OK;
    }

  }
  // add end
  // add by kulw at 2013.12.12
  else if (strncmp(packet, "QSetSTDIN", 9) == 0) {
    gdb_put_packet(connection, "OK", 2);
    return ERROR_OK;

  }
  else if (strncmp(packet, "QSetSTDOUT", 10) == 0) {
    gdb_put_packet(connection, "OK", 2);
    return ERROR_OK;

  }
  else if (strncmp(packet, "QSetSTDERR", 10) == 0) {
    gdb_put_packet(connection, "OK", 2);
    return ERROR_OK;

  }

  else if (strncmp(packet, "QSetDisableASLR", 15) == 0) {
    gdb_put_packet(connection, "OK", 2);
    return ERROR_OK;

  }

  else if (strncmp(packet, "qProcessInfo", 12) == 0) {

    char processInfo[1024];
    processInfo[sizeof(processInfo) - 1] = '\0';

    /*typedef struct {
        char *key;
        int value;
    }PrcsPair;

    PrcsPair KV[6]={
    {"pid:",           getpid()},
    {";parent-pid:",   getppid()},
    {";real-uid:",     getuid()},
    {";real-gid:",     getgid()},
    {";effective-uid:",geteuid()},
    {";effective-gid:",getegid()}
    };

    int i, len = 0, size = sizeof(processInfo);
    for(i = 0; i < 6; i++){
        char str[10];
        char hexstr[20];
        snprintf(str,sizeof(str),"%d",KV[i].value);
        hexify(hexstr, str, 0, sizeof(hexstr));
        snprintf(processInfo +len, size -len, "%s%s", KV[i].key, hexstr);
        len = strlen(processInfo);
        size = sizeof(processInfo);
    }
    snprintf(processInfo +len, size -len, "%s", ";");

    gdb_put_packet(connection, processInfo, strlen(processInfo));*/


    int fd;
    int ppid = 0;
    int rUid = 0;
    int rGid = 0;
    int eUid = 0;
    int eGid = 0;

    char path[PATH_MAX];

    char* buf_sp = (char *)malloc(1024);
    if (snprintf (path, PATH_MAX, "/proc/%" PRIu64 "/status", target->pid) > 0)
    {
        if ((fd = open (path, O_RDONLY, 0)) >= 0)
        {//printf("opened\n");
          int bytes_read = 0;
          for(;;)
          {
            int avail = sizeof(buf_sp) - bytes_read;
            int status = read (fd, buf_sp + bytes_read, avail);
            if (status < 0) break;
            if (status == 0)
            {
                buf_sp[bytes_read] = '\0';
                break;
            }
            bytes_read += status;
          }
        }
        close (fd);
    }
    char *buf_ppid = strstr((char *)buf_sp, "PPid:");
    if (buf_ppid)
    {
      buf_ppid += sizeof("PPid:");
      ppid = strtol (buf_ppid, &buf_ppid, 10);
    }

    char *buf_uid = strstr ((char *)buf_sp, "Uid:");
    if (buf_uid)
    {
        // Real, effective, saved set, and file system UIDs. Read the first two.
        buf_uid += sizeof("Uid:");
        rUid = strtol (buf_uid, &buf_uid, 10);
        eUid = strtol (buf_uid, &buf_uid, 10);
    }

    char *buf_gid = strstr ((char *)buf_sp, "Gid:");
    if (buf_gid)
    {
        // Real, effective, saved set, and file system GIDs. Read the first two.
        buf_gid += sizeof("Gid:");
        rGid = strtol (buf_gid, &buf_gid, 10);
        eGid = strtol (buf_gid, &buf_gid, 10);
    }
    free(buf_sp);
    snprintf(processInfo, sizeof(processInfo),
             "pid:%d;parent-pid:%d;real-uid:%d;real-gid:%d;effective-uid:%d;effective-gid:%d;",
             target->pid, ppid, rUid, rGid, eUid, eGid);
    processInfo[strlen(processInfo)] = '\0';
    gdb_put_packet(connection, processInfo, strlen(processInfo));
    return ERROR_OK;
  }
  else if (strncmp(packet, "qProcessInfoPID", 15) == 0) {
    if (target->pid == atoi(packet + 16))
      gdb_put_packet(connection, "OK", 2);//
    else
      gdb_put_packet(connection, "", 0);//not sure

    return ERROR_OK;
  }
  else if (strncmp(packet, "qfThreadInfo", 12) == 0) {
    char str[10];
    int sz = sprintf(str, "m%X", target->tid);
    gdb_put_packet(connection, str, sz);
    return ERROR_OK;
  }
  else if (strncmp(packet, "qsThreadInfo", 12) == 0) {
    gdb_put_packet(connection, "l", 1);
    return ERROR_OK;
  }
  // add end

  // no handle for query packet, so we send a null reply not an error packet.
  //gdb_send_error(connection, 1);
  gdb_put_packet(connection, "", 0);

  return ERROR_OK;
}

static int gdb_input_inner(struct connection *connection)
{
	/* Do not allocate this on the stack */
	static char gdb_packet_buffer[GDB_BUFFER_SIZE];

	struct gdb_service *gdb_service = connection->service->priv;
	struct target *target = gdb_service->target;
	struct target_type * target_impl = target->type;

	char *packet = gdb_packet_buffer;
	int packet_size;
	int retval;
	struct gdb_connection *gdb_con = connection->priv;
	static int extended_protocol = 0;

	/* drain input buffer. If one of the packets fail, then an error
	 * packet is replied, if applicable.
	 *
	 * This loop will terminate and the error code is returned.
	 *
	 * The calling fn will check if this error is something that
	 * can be recovered from, or if the connection must be closed.
	 *
	 * If the error is recoverable, this fn is called again to
	 * drain the rest of the buffer.
	 */
	do
	{
		packet_size = GDB_BUFFER_SIZE-1;
		retval = gdb_get_packet(connection, packet, &packet_size);

		if (retval != ERROR_OK)
			return retval;

		/* terminate with zero */
		packet[packet_size] = 0;

		if (LOG_LEVEL_IS(LOG_LVL_DEBUG)) {
			if (packet[0] == 'X') {
				// binary packets spew junk into the debug log stream
				char buf[ 50 ];
				int x;
				for (x = 0 ; (x < 49) && (packet[x] != ':') ; x++) {
					buf[x] = packet[x];
				}
				buf[x] = 0;
				LOG_DEBUG("received packet: '%s:<binary-data>'", buf);
			} else {
				LOG_DEBUG("received packet: '%s'", packet);
			}
		}

		if (packet_size > 0)
		{
			retval = ERROR_OK;
			switch (packet[0])
			{
        case 'A':
          // mapu spu todo:
          // prepare args via memory write
          gdb_put_packet(connection, "OK", 2);
          break;

			  case 'T': // Is thread alive?
			   	//gdb_thread_packet(connection, target, packet, packet_size);
			  	break;
			  case 'H': // Set current thread ( 'c' for step and continue, 'g' for all other operations )
			   	//gdb_thread_packet(connection, target, packet, packet_size);
			   	break;
				case 'q':
				case 'Q':
					/*retval = gdb_thread_packet(connection,
												target, packet,
												packet_size);
					if ( retval == GDB_THREAD_PACKET_NOT_CONSUMED )*/
					{
						retval = gdb_query_packet(connection, packet, packet_size);
					}
					break;

				case 'g':
					retval = gdb_get_registers_packet(connection, target, packet, packet_size);
					break;
				case 'G':
					retval = gdb_set_registers_packet(connection, target, packet, packet_size);
					break;
				case 'p':
					retval = gdb_get_register_packet(connection, target, packet, packet_size);
					break;
				case 'P':
					retval = gdb_set_register_packet(connection, target, packet, packet_size);
					break;
				case 'm':
					retval = gdb_read_memory_packet(connection, target, packet, packet_size);
					break;
				case 'M':
					retval = gdb_write_memory_packet(connection, target, packet, packet_size);
					break;
				case 'z':
				case 'Z':
					retval = gdb_breakpoint_watchpoint_packet(connection, target, packet, packet_size);
					break;
				case '?':
					gdb_stop_reply_packet(connection, target, packet, packet_size);
					break;
				case 'c':
				case 's':
					{
						log_add_callback(gdb_log_callback, connection);

						if (gdb_con->mem_write_error)
						{
							LOG_ERROR("Memory write failure!");

							/* now that we have reported the memory write error, we can clear the condition */
							gdb_con->mem_write_error = false;
						}

						bool nostep = false;
						bool already_running = false;
						if (target->state == TARGET_RUNNING)
						{
							LOG_WARNING("WARNING! The target is already running. "
									"All changes GDB did to registers will be discarded! "
									"Waiting for target to halt.");
							already_running = true;
						}
						else if (target->state != TARGET_HALTED)
						{
				      nostep = true;
							LOG_WARNING("The target is not in the halted nor running stated, stepi/continue ignored.");
						}
						else if ((packet[0] == 's') && gdb_con->sync)
						{
							/* Hmm..... when you issue a continue in GDB, then a "stepi" is
							 * sent by GDB first to OpenOCD, thus defeating the check to
							 * make only the single stepping have the sync feature...
							 */
							nostep = true;
							LOG_WARNING("stepi ignored. GDB will now fetch the register state from the target.");
						}

						gdb_con->sync = false;

						if (!already_running && nostep)
						{
						    // we simply ignore the first "c"/"s" packet.
						    target->debug_reason == DBG_REASON_DBGRQ;

							/* Either the target isn't in the halted state, then we can't
							 * step/continue. This might be early setup, etc.
							 *
							 * Or we want to allow GDB to pick up a fresh set of
							 * register values without modifying the target state.
							 */

							gdb_stop_reply_packet(connection, target, packet, packet_size);

							/* stop forwarding log packets! */
							log_remove_callback(gdb_log_callback, connection);
						}
						else
						{
							/* We're running/stepping, in which case we can
							 * forward log output until the target is halted
							 */
							gdb_con->frontend_state = TARGET_RUNNING;
							target_call_event_callbacks(target, TARGET_EVENT_GDB_START);

							if (!already_running)
							{
								/* Here we don't want packet processing to stop even if this fails,
								 * so we use a local variable instead of retval. */
								retval = gdb_step_continue_packet(connection, target, packet, packet_size);
								if (retval != ERROR_OK)
								{
									/* we'll never receive a halted condition... issue a false one.. */
									gdb_frontend_halted(target, connection);
								}

								// we then poll until target is halted.
								int i = 0;
								do {
								  target_poll(target);
								  if(target->state == TARGET_HALTED) break;
								  i++;
								  if(i > 0) usleep(100000);
								  if(i > 60000)
								  {
								    LOG_ERROR("timeout to wait for halt after continue/step");
								    gdb_send_error(connection, 2);
								    return ERROR_TARGET_TIMEOUT;
								  }

								} while(1);

								// now target is halted, we send back a stop reply packet.
								gdb_stop_reply_packet(connection, target, packet, packet_size);

							}
						}
					}
					break;
				case 'v':
					retval = gdb_v_packet(connection, target, packet, packet_size);
					break;
				case 'D':
					retval = gdb_detach(connection, target);
					extended_protocol = 0;
					break;
				case 'X':
					retval = gdb_write_memory_binary_packet(connection, target, packet, packet_size);
					if (retval != ERROR_OK)
						return retval;
					break;
				case 'k':
					if (extended_protocol != 0)
						break;
					gdb_put_packet(connection, "OK", 2);
					return ERROR_SERVER_REMOTE_CLOSED;
				case '!':
					/* handle extended remote protocol */
					extended_protocol = 1;
					gdb_put_packet(connection, "OK", 2);
					break;
				case 'R':
					/* handle extended restart packet */
					breakpoint_clear_target(gdb_service->target);
					watchpoint_clear_target(gdb_service->target);
					command_run_linef(connection->cmd_ctx, "ocd_gdb_restart %s", target_name(target));
					gdb_put_packet(connection, "OK", 2);
					break;

				case 'j':
				    /*  packet supported only by smp target i.e cortex_a.c*/
					/* handle smp packet replying coreid played to gbd */
					// gdb_read_smp_packet(connection, target,	packet, packet_size);
					break;

				case 'J':
					/*  packet supported only by smp target i.e cortex_a.c */
					/*  handle smp packet setting coreid to be played at next
					 *  resume to gdb */
					// gdb_write_smp_packet(connection, target,packet, packet_size);
					break;

				default:
					/* ignore unknown packets */
					LOG_DEBUG("ignoring 0x%2.2x packet", packet[0]);
					gdb_put_packet(connection, NULL, 0);
					break;
			}

			/* if a packet handler returned an error, exit input loop */
			if (retval != ERROR_OK)
				return retval;
		}

		if (gdb_con->ctrl_c)
		{
			if (target->state == TARGET_RUNNING)
			{
				retval = target_halt(target);
				if (retval != ERROR_OK)
				{
					target_call_event_callbacks(target, TARGET_EVENT_GDB_HALT);
				}
				gdb_con->ctrl_c = 0;
			} else
			{
				LOG_INFO("The target is not running when halt was requested, stopping GDB.");
				target_call_event_callbacks(target, TARGET_EVENT_GDB_HALT);
			}
		}

	} while (gdb_con->buf_cnt > 0);

	return ERROR_OK;
}

static int gdb_input(struct connection *connection)
{
	int retval = gdb_input_inner(connection);
	struct gdb_connection *gdb_con = connection->priv;
	if (retval == ERROR_SERVER_REMOTE_CLOSED)
		return retval;

	/* logging does not propagate the error, yet can set the gdb_con->closed flag */
	if (gdb_con->closed)
		return ERROR_SERVER_REMOTE_CLOSED;

	/* we'll recover from any other errors(e.g. temporary timeouts, etc.) */
	return ERROR_OK;
}

static int gdb_target_start(struct target *target, const char *port)
{
  struct gdb_service *gdb_service;
  int ret;
  gdb_service = malloc(sizeof(struct gdb_service));

  if (NULL == gdb_service)
    return -ENOMEM;

  gdb_service->target = target;
  gdb_service->core[0] = -1;
  gdb_service->core[1] = -1;
  target->gdb_service = gdb_service;

  char service_desc[128];
  snprintf(service_desc, sizeof(service_desc),
           "gdb-compatible debug service for target %s", target->cmd_name);

  ret = add_service(service_desc, port, 1, &gdb_new_connection, &gdb_input,
                    &gdb_connection_closed, gdb_service, &target->gdb_port);

  // fake a process ID with its gdb port.
  target->pid = target->gdb_port;
  target->tid = 1;
  target->debug_reason = DBG_REASON_DBGRQ;

  /* initialialize all gdb services of targets in one target_list with the same one */
  {
    struct target_list *head = target->head;
    struct target *curr;
    while (head != (struct target_list *)NULL)
    {
      curr = head->target;
      if (curr != target)
        curr->gdb_service = gdb_service;

      head = head->next;
    }
  }

  return ret;
}

static int gdb_target_add_one(struct target *target)
{
	/*  one gdb instance per smp list */
	if ((target->smp) && (target->gdb_service)) return ERROR_OK;
	int retval = gdb_target_start(target, gdb_port_next);
	if (retval == ERROR_OK)
	{
		long portnumber;
		/* If we can parse the port number
		 * then we increment the port number for the next target.
		 */
		char *end;
		portnumber = strtol(gdb_port_next, &end, 0);
		if (!*end)
		{
			if (parse_long(gdb_port_next, &portnumber) == ERROR_OK)
			{
				free((void *)gdb_port_next);
				gdb_port_next = alloc_printf("%d", portnumber+1);
			}
		}
	}
	return retval;
}

int gdb_target_add_all(struct target *target)
{
	if (NULL == target)
	{
		LOG_WARNING("gdb services need one or more targets defined");
		return ERROR_OK;
	}

  int retval;

  // Note: this is what needed to be done for lldb platform debug server.
  /*struct gdb_service *gdb_service = malloc(sizeof(struct gdb_service));

  if (NULL == gdb_service)
    return -ENOMEM;

  // since this is a daemon, we let it be the head of the target list.
  gdb_service->target = target;
  gdb_service->core[0] = -1;
  gdb_service->core[1] = -1;



	// MaPU SPU hacking: at first we add a service daemon for lldb
  retval = add_service("gdb-compatible debug daemon", gdb_port_next, 1,
                           &gdb_new_connection, &gdb_input, &gdb_connection_closed,
                           gdb_service, NULL);

  if (ERROR_OK != retval)
    return retval;*/

  // construct a gdb service for each of all targets
	while (NULL != target)
	{
		retval = gdb_target_add_one(target);
		if (ERROR_OK != retval)
			return retval;

		target = target->next;
	}

	return ERROR_OK;
}

COMMAND_HANDLER(handle_gdb_sync_command)
{
	if (CMD_ARGC != 0)
	{
		return ERROR_COMMAND_SYNTAX_ERROR;
	}

	if (current_gdb_connection == NULL)
	{
		command_print(CMD_CTX,
				"gdb_sync command can only be run from within gdb using \"monitor gdb_sync\"");
		return ERROR_FAIL;
	}

	current_gdb_connection->sync = true;

	return ERROR_OK;
}

/* daemon configuration command gdb_port */
COMMAND_HANDLER(handle_gdb_port_command)
{
	int retval = CALL_COMMAND_HANDLER(server_pipe_command, &gdb_port);
	if (ERROR_OK == retval) {
		free((void*)gdb_port_next);
		gdb_port_next = strdup(gdb_port);
	}
	return retval;
}

COMMAND_HANDLER(handle_gdb_memory_map_command)
{
	if (CMD_ARGC != 1)
		return ERROR_COMMAND_SYNTAX_ERROR;

	COMMAND_PARSE_ENABLE(CMD_ARGV[0], gdb_use_memory_map);
	return ERROR_OK;
}

COMMAND_HANDLER(handle_gdb_flash_program_command)
{
	if (CMD_ARGC != 1)
		return ERROR_COMMAND_SYNTAX_ERROR;

	COMMAND_PARSE_ENABLE(CMD_ARGV[0], gdb_flash_program);
	return ERROR_OK;
}

COMMAND_HANDLER(handle_gdb_report_data_abort_command)
{
	if (CMD_ARGC != 1)
		return ERROR_COMMAND_SYNTAX_ERROR;

	COMMAND_PARSE_ENABLE(CMD_ARGV[0], gdb_report_data_abort);
	return ERROR_OK;
}

/* gdb_breakpoint_override */
COMMAND_HANDLER(handle_gdb_breakpoint_override_command)
{
	if (CMD_ARGC == 0)
	{

	} else if (CMD_ARGC == 1)
	{
		gdb_breakpoint_override = 1;
		if (strcmp(CMD_ARGV[0], "hard") == 0)
		{
			gdb_breakpoint_override_type = BKPT_HARD;
		} else if (strcmp(CMD_ARGV[0], "soft") == 0)
		{
			gdb_breakpoint_override_type = BKPT_SOFT;
		} else if (strcmp(CMD_ARGV[0], "disable") == 0)
		{
			gdb_breakpoint_override = 0;
		}
	} else
	{
		return ERROR_COMMAND_SYNTAX_ERROR;
	}
	if (gdb_breakpoint_override)
	{
		LOG_USER("force %s breakpoints", (gdb_breakpoint_override_type == BKPT_HARD)?"hard":"soft");
	} else
	{
		LOG_USER("breakpoint type is not overridden");
	}

	return ERROR_OK;
}

static const struct command_registration gdb_command_handlers[] = {
	{
		.name = "gdb_sync",
		.handler = handle_gdb_sync_command,
		.mode = COMMAND_ANY,
		.help = "next stepi will return immediately allowing "
			"GDB to fetch register state without affecting "
			"target state",
	},
	{
		.name = "gdb_port",
		.handler = handle_gdb_port_command,
		.mode = COMMAND_ANY,
		.help = "Normally gdb listens to a TCP/IP port. Each subsequent GDB "
				"server listens for the next port number after the "
				"base port number specified. "
				"No arguments reports GDB port. \"pipe\" means listen to stdin "
				"output to stdout, an integer is base port number, \"disable\" disables "
				"port. Any other string is are interpreted as named pipe to listen to. "
				"Output pipe is the same name as input pipe, but with 'o' appended.",
		.usage = "[port_num]",
	},
	{
		.name = "gdb_memory_map",
		.handler = handle_gdb_memory_map_command,
		.mode = COMMAND_CONFIG,
		.help = "enable or disable memory map",
		.usage = "('enable'|'disable')"
	},
	{
		.name = "gdb_flash_program",
		.handler = handle_gdb_flash_program_command,
		.mode = COMMAND_CONFIG,
		.help = "enable or disable flash program",
		.usage = "('enable'|'disable')"
	},
	{
		.name = "gdb_report_data_abort",
		.handler = handle_gdb_report_data_abort_command,
		.mode = COMMAND_CONFIG,
		.help = "enable or disable reporting data aborts",
		.usage = "('enable'|'disable')"
	},
	{
		.name = "gdb_breakpoint_override",
		.handler = handle_gdb_breakpoint_override_command,
		.mode = COMMAND_ANY,
		.help = "Display or specify type of breakpoint "
			"to be used by gdb 'break' commands.",
		.usage = "('hard'|'soft'|'disable')"
	},
	COMMAND_REGISTRATION_DONE
};

int gdb_register_commands(struct command_context *cmd_ctx)
{
	gdb_port = strdup("3333");
	gdb_port_next = strdup("3333");
	return register_commands(cmd_ctx, NULL, gdb_command_handlers);
}
