/***************************************************************************
 *   Copyright (C) 2008 by Øyvind Harboe                                 *
 *   oyvind.harboe@zylin.com                                               *
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

#include <jtag/interface.h>
#include <bitbang.h>
#include <helper/log.h>

struct command_registration;

COMMAND_HANDLER(handle_foo_command)
{
	if (CMD_ARGC < 1 || CMD_ARGC > 2)
	{
		LOG_ERROR("%s: incorrect number of arguments", CMD_NAME);
		return ERROR_COMMAND_SYNTAX_ERROR;
	}

	uint32_t address;
	COMMAND_PARSE_NUMBER(u32, CMD_ARGV[0], address);

	const char *msg = "<unchanged>";
	if (CMD_ARGC == 2)
	{
		bool enable;
		COMMAND_PARSE_ENABLE(CMD_ARGV[1], enable);
		msg = enable ? "enable" : "disable";
	}

	LOG_INFO("%s: address=0x%8.8" PRIx32 " enabled=%s", CMD_NAME, address, msg);
	return ERROR_OK;
}

static bool foo_flag;

COMMAND_HANDLER(handle_flag_command)
{
	return CALL_COMMAND_HANDLER(handle_command_parse_bool,
			&foo_flag, "foo flag");
}

static const struct command_registration foo_command_handlers[] = {
	{
		.name = "bar",
		.handler = &handle_foo_command,
		.mode = COMMAND_ANY,
		.usage = "address ['enable'|'disable']",
		.help = "an example command",
	},
	{
		.name = "baz",
		.handler = &handle_foo_command,
		.mode = COMMAND_ANY,
		.usage = "address ['enable'|'disable']",
		.help = "a sample command",
	},
	{
		.name = "flag",
		.handler = &handle_flag_command,
		.mode = COMMAND_ANY,
		.usage = "[on|off]",
		.help = "set a flag",
	},
	COMMAND_REGISTRATION_DONE
};

static COMMAND_HELPER(handle_hello_args, const char **sep, const char **name)
{
	if (CMD_ARGC > 1)
	{
		LOG_ERROR("%s: too many arguments", CMD_NAME);
		return ERROR_COMMAND_SYNTAX_ERROR;
	}
	if (1 == CMD_ARGC)
	{
		*sep = " ";
		*name = CMD_ARGV[0];
	}
	else
		*sep = *name = "";

	return ERROR_OK;
}

COMMAND_HANDLER(handle_hello_command)
{
	const char *sep, *name;
	int retval = CALL_COMMAND_HANDLER(handle_hello_args, &sep, &name);
	if (ERROR_OK == retval)
		command_print(CMD_CTX, "Greetings%s%s!", sep, name);
	return retval;
}

const struct command_registration hello_command_handlers[] = {
	{
		.name = "hello",
		.handler = handle_hello_command,
		.mode = COMMAND_ANY,
		.help = "prints a warm welcome",
		.usage = "[name]",
	},
	{
		.name = "foo",
		.mode = COMMAND_ANY,
		.help = "example command handler skeleton",

		.chain = foo_command_handlers,
	},
	COMMAND_REGISTRATION_DONE
};

/* my private tap controller state, which tracks state for calling code */
static tap_state_t dummy_state = TAP_RESET;

static int dummy_clock;         /* edge detector */

static int clock_count;         /* count clocks in any stable state, only stable states */

static uint32_t dummy_data;

static int dummy_read(void)
{
	int data = 1 & dummy_data;
	dummy_data = (dummy_data >> 1) | (1 << 31);
	return data;
}

static void dummy_write(int tck, int tms, int tdi)
{
	/* TAP standard: "state transitions occur on rising edge of clock" */
	if (tck != dummy_clock)
	{
		if (tck)
		{
			tap_state_t old_state = dummy_state;
			dummy_state = tap_state_transition(old_state, tms);

			if (old_state != dummy_state)
			{
				if (clock_count)
				{
					LOG_DEBUG("dummy_tap: %d stable clocks", clock_count);
					clock_count = 0;
				}

				LOG_DEBUG("dummy_tap: %s", tap_state_name(dummy_state));

#if defined(DEBUG)
				if (dummy_state == TAP_DRCAPTURE)
					dummy_data = 0x01255043;
#endif
			}
			else
			{
				/* this is a stable state clock edge, no change of state here,
				 * simply increment clock_count for subsequent logging
				 */
				++clock_count;
			}
		}
		dummy_clock = tck;
	}
}

static void dummy_reset(int trst, int srst)
{
	dummy_clock = 0;

	if (trst || (srst && (jtag_get_reset_config() & RESET_SRST_PULLS_TRST)))
		dummy_state = TAP_RESET;

	LOG_DEBUG("reset to: %s", tap_state_name(dummy_state));
}

static void dummy_led(int on)
{
}

static struct bitbang_interface dummy_bitbang_interface = {
		.read = &dummy_read,
		.write = &dummy_write,
		.reset = &dummy_reset,
		.blink = &dummy_led,
	};

static int dummy_khz(int khz, int *jtag_speed)
{
	if (khz == 0)
	{
		*jtag_speed = 0;
	}
	else
	{
		*jtag_speed = 64000/khz;
	}
	return ERROR_OK;
}

static int dummy_speed_div(int speed, int *khz)
{
	if (speed == 0)
	{
		*khz = 0;
	}
	else
	{
		*khz = 64000/speed;
	}

	return ERROR_OK;
}

static int dummy_speed(int speed)
{
	return ERROR_OK;
}

static int dummy_init(void)
{
	bitbang_interface = &dummy_bitbang_interface;

	return ERROR_OK;
}

static int dummy_quit(void)
{
	return ERROR_OK;
}

static const struct command_registration dummy_command_handlers[] = {
	{
		.name = "dummy",
		.mode = COMMAND_ANY,
		.help = "dummy interface driver commands",

		.chain = hello_command_handlers,
	},
	COMMAND_REGISTRATION_DONE,
};

/* The dummy driver is used to easily check the code path
 * where the target is unresponsive.
 */
struct jtag_interface dummy_interface = {
		.name = "dummy",

		.supported = DEBUG_CAP_TMS_SEQ,
		.commands = dummy_command_handlers,
		.transports = jtag_only,

		.execute_queue = &bitbang_execute_queue,

		.speed = &dummy_speed,
		.khz = &dummy_khz,
		.speed_div = &dummy_speed_div,

		.init = &dummy_init,
		.quit = &dummy_quit,
};
