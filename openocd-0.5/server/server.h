/***************************************************************************
 *   Copyright (C) 2005 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *                                                                         *
 *   Copyright (C) 2007,2008 Øyvind Harboe                                 *
 *   oyvind.harboe@zylin.com                                               *
 *                                                                         *
 *   Copyright (C) 2008 by Spencer Oliver                                  *
 *   spen@spen-soft.co.uk                                                  *
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
#ifndef SERVER_H
#define SERVER_H

#include <helper/log.h>

#include <netinet/in.h>

enum connection_type
{
	CONNECTION_TCP,
	CONNECTION_PIPE,
	CONNECTION_STDINOUT
};

struct connection
{
	int fd;
	int fd_out; /* When using pipes we're writing to a different fd */
	struct sockaddr_in sin;
	struct command_context *cmd_ctx;
	struct service *service;
	int input_pending;
	void *priv;
	struct connection *next;
};

typedef int (*new_connection_handler_t)(struct connection *connection);
typedef int (*input_handler_t)(struct connection *connection);
typedef int (*connection_closed_handler_t)(struct connection *connection);

struct service
{
	const char *name;
	enum connection_type type;
	const char *port;
	unsigned short portnumber;
	int fd;
	struct sockaddr_in sin;
	int max_connections;
	struct connection *connections;
	new_connection_handler_t new_connection;
	input_handler_t input;
	connection_closed_handler_t connection_closed;
	void *priv;
	struct service *next;
};
struct service * get_telnet_service(void);
int add_service(char *name, const char *port,
		int max_connections, new_connection_handler_t new_connection_handler,
		input_handler_t in_handler, connection_closed_handler_t close_handler,
		void *priv, uint16_t * true_port);

int server_preinit(void);
int server_init(struct command_context *cmd_ctx);
int server_quit(void);

int server_loop(struct command_context *command_context);

int server_register_commands(struct command_context *context);

int connection_write(struct connection *connection, const void *data, int len);
int connection_read(struct connection *connection, void *data, int len);

/**
 * Used by server_loop(), defined in server_stubs.c or ecosboard.c
 */
void openocd_sleep_prelude(void);
/**
 * Used by server_loop(), defined in server_stubs.c or ecosboard.c
 */
void openocd_sleep_postlude(void);

/**
 * Defines an extended command handler function declaration to enable
 * access to (and manipulation of) the server port number.
 * Call server_port like a normal COMMAND_HANDLER with an extra @a out parameter
 * to receive the specified port number.
 */
#define SERVER_PIPE_COMMAND() \
		COMMAND_HELPER(server_pipe_command, const char **out)
SERVER_PIPE_COMMAND();

#define SERVER_PORT_COMMAND() \
		COMMAND_HELPER(server_port_command, unsigned short *out)

SERVER_PORT_COMMAND();

#define ERROR_SERVER_REMOTE_CLOSED	(-400)
#define ERROR_CONNECTION_REJECTED	(-401)

#endif /* SERVER_H */
