/***************************************************************************
 *   Copyright (C) 2005 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *                                                                         *
 *   Copyright (C) 2007-2010 Øyvind Harboe                                 *
 *   oyvind.harboe@zylin.com                                               *
 *                                                                         *
 *   Copyright (C) 2008 Richard Missenden                                  *
 *   richard.missenden@googlemail.com                                      *
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

#include "config.h"

#include "openocd.h"
#include <jtag/driver.h>
#include <jtag/jtag.h>
#include <transport/transport.h>
#include <helper/ioutil.h>
#include <helper/util.h>
#include <helper/configuration.h>
#include <helper/command.h>
#include <target/target.h>
#include <string.h>
#include <server/server.h>
#include <server/gdb_server.h>

#include <strings.h>
#include <netinet/in.h>
#include <jtag/drivers/fifo.h>
#include <libgen.h>

#define OPENOCD_VERSION \
		"Open On-Chip Debugger "

/* Give scripts and TELNET a way to find out what version this is */
static int
jim_version_command(Jim_Interp *interp, int argc, Jim_Obj * const *argv)
{
	if(argc > 2) {
		return JIM_ERR;
	}
	const char *str = "";
	char * version_str;
	version_str = OPENOCD_VERSION;

	if(argc == 2)
		str = Jim_GetString(argv[1], NULL);

	Jim_SetResult(interp, Jim_NewStringObj(interp, version_str, -1));

	return JIM_OK;
}

static int
log_target_callback_event_handler(struct target *target, enum target_event event, void *priv)
{
//	switch (event)
//	{
//		case TARGET_EVENT_GDB_START:
//			target->display = 0;
//			break;
//		case TARGET_EVENT_GDB_END:
//			target->display = 1;
//			break;
//		case TARGET_EVENT_HALTED:
//			if (target->display)
//			{
//				/* do not display information when debugger caused the halt */
//				target_arch_state(target);
//			}
//			break;
//		default:
//			break;
//	}

	return ERROR_OK;
}

static bool init_at_startup = true;

COMMAND_HANDLER(handle_noinit_command)
{
	if(CMD_ARGC != 0)
		return ERROR_COMMAND_SYNTAX_ERROR;
	init_at_startup = false;
	return ERROR_OK;
}

/* OpenOCD can't really handle failure of this command. Patches welcome! :-) */
COMMAND_HANDLER(handle_init_command)
{

	if(CMD_ARGC != 0)
		return ERROR_COMMAND_SYNTAX_ERROR;

	int retval;
	static int initialized = 0;
	if(initialized)
		return ERROR_OK;

	initialized = 1;

	retval = command_run_line(CMD_CTX, "target init");
	if(ERROR_OK != retval)
		return ERROR_FAIL;
#ifndef SIM
	if ((retval = adapter_init(CMD_CTX)) != ERROR_OK)
	{
		/* we must be able to set up the debug adapter */
		return retval;
	}

	LOG_DEBUG("Debug Adapter init complete");

	/* "transport init" verifies the expected devices are present;
	 * for JTAG, it checks the list of configured TAPs against
	 * what's discoverable, possibly with help from the platform's
	 * JTAG event handlers.  (which require COMMAND_EXEC)
	 */
	command_context_mode(CMD_CTX, COMMAND_EXEC);
	retval = command_run_line(CMD_CTX, "transport init");
	if (ERROR_OK != retval)
		return ERROR_FAIL;
#endif

	LOG_DEBUG("Examining targets...");
	if(target_examine() != ERROR_OK)
		LOG_DEBUG("target examination failed");

	command_context_mode(CMD_CTX, COMMAND_CONFIG);

	command_context_mode(CMD_CTX, COMMAND_EXEC);

	/* initialize telnet subsystem */
	gdb_target_add_all(all_targets);

	target_register_event_callback(log_target_callback_event_handler, CMD_CTX);

	return ERROR_OK;
}

COMMAND_HANDLER(handle_add_script_search_dir_command)
{
	if(CMD_ARGC != 1)
		return ERROR_COMMAND_SYNTAX_ERROR;

	add_script_search_dir(CMD_ARGV[0]);

	return ERROR_OK;
}

static int
jim_stacktrace_command(Jim_Interp *interp, int argc, Jim_Obj * const *argv)
{
	if(argc != 1) {
		return JIM_ERR;
	}
	Jim_Obj * stacktrace = Jim_DuplicateObj(interp, interp->stackTrace);

	/* insert actual error site at beginning of list*/
	Jim_Obj *procname = Jim_NewStringObj(interp, "", -1); /* Uhhh... don't know this one. */
	Jim_ListInsertElements(interp, stacktrace, 0, 1, &procname);
	Jim_Obj *filename = Jim_NewStringObj(interp, interp->errorFileName, -1);
	Jim_ListInsertElements(interp, stacktrace, 1, 1, &filename);
	Jim_Obj *line = Jim_NewIntObj(interp, interp->errorLine);
	Jim_ListInsertElements(interp, stacktrace, 2, 1, &line);

	Jim_SetResult(interp, stacktrace);

	return JIM_OK;
}

static const struct command_registration openocd_command_handlers[] = {
		{.name = "version", .jim_handler = jim_version_command, .mode = COMMAND_ANY, .help = "show program version", }, {
				.name = "noinit", .handler = &handle_noinit_command, .mode = COMMAND_CONFIG,
				.help = "Prevent 'init' from being called at startup.", },
		{.name = "init", .handler = &handle_init_command, .mode = COMMAND_ANY,
			.help = "Initializes configured targets and servers.  "
					"Changes command mode from CONFIG to EXEC.  "
					"Unless 'noinit' is called, this command is "
					"called automatically at the end of startup.",

		},
		{.name = "add_script_search_dir", .handler = &handle_add_script_search_dir_command, .mode = COMMAND_ANY,
			.help = "dir to search for config files and scripts",

		},
		{.name = "stacktrace", .jim_handler = jim_stacktrace_command, .mode = COMMAND_ANY,
			.help = "returns the stacktrace as a list of triples: proc, file, line."
					"The stack trace is reset when a new stack trace is being built after "
					"a new failure has occurred.", },
		COMMAND_REGISTRATION_DONE};

static int
openocd_register_commands(struct command_context *cmd_ctx)
{
	return register_commands(cmd_ctx, NULL, openocd_command_handlers);
}

struct command_context *global_cmd_ctx;

/* NB! this fn can be invoked outside this file for non PC hosted builds
 * NB! do not change to 'static'!!!!
 */
struct command_context *
setup_command_handler(Jim_Interp *interp)
{
	log_init();

	LOG_DEBUG("log_init: complete");

	char *path = calloc(1,128);

	int k = readlink("/proc/self/exe", path, 128);
	dirname(path);
	strcat(path, "/startup.tcl");

	struct command_context *cmd_ctx = command_init(path, interp);
	free(path);

	/* register subsystem commands */
	typedef int
	(*command_registrant_t)(struct command_context *cmd_ctx_value);
	static const command_registrant_t command_registrants[] = {
		&openocd_register_commands,
		&server_register_commands,
		&gdb_register_commands,
		&log_register_commands,
		&transport_register_commands,
		&interface_register_commands,
		&target_register_commands,
		//&flash_register_commands,
		//&nand_register_commands,
		//&pld_register_commands,
		//&mflash_register_commands,
		NULL
	};
	for(unsigned i = 0; NULL != command_registrants[i]; i++) {
		int retval = (*command_registrants[i])(cmd_ctx);
		if(ERROR_OK != retval) {
			command_done(cmd_ctx);
			return NULL;
		}
	}

	LOG_DEBUG("command registration: complete");

	LOG_OUTPUT(OPENOCD_VERSION "\n"	"Licensed under GNU GPL v2\n");

	global_cmd_ctx = cmd_ctx;

	return cmd_ctx;
}

extern const char* simulator_path;
/** OpenOCD runtime meat that can become single-thread in future. It parse
 * commandline, reads configuration, sets up the target and starts server loop.
 * Commandline arguments are passed into this function from openocd_main().
 */
static int
openocd_thread(int argc, char *argv[], struct command_context *cmd_ctx)
{
	int ret;

	if(parse_cmdline_args(cmd_ctx, argc, argv) != ERROR_OK)
		return EXIT_FAILURE;

	if(server_preinit() != ERROR_OK)
		return EXIT_FAILURE;

	ret = parse_config_file(cmd_ctx);
	if(ret != ERROR_OK)
		return EXIT_FAILURE;

	// initialize tcl and telnet server.
	ret = server_init(cmd_ctx);
	if(ERROR_OK != ret)
		return EXIT_FAILURE;

	/*struct service * svc = get_telnet_service();
	if(svc!=NULL)
		LOG_INFO("telnet server is listening at port %d...", svc->portnumber);*/

	// "init_targets" is a tcl proc which performs nothing.
	ret = command_run_line(cmd_ctx, "init_targets");
	if(ERROR_OK != ret)
		ret = EXIT_FAILURE;

	if(init_at_startup) {
		ret = command_run_line(cmd_ctx, "init");
		if(ERROR_OK != ret)
			return EXIT_FAILURE;
	}

#ifdef SIM
	if(simulator_path == NULL)
		LOG_INFO("You did not specify the simulator, use \"-e exec-path\". openocd will run independently.");

	ret = init_fifo_client_ext(simulator_path);
#endif

	server_loop(cmd_ctx);

#ifdef SIM
	ret = deinit_fifo_client();
#endif

	server_quit();

	return ret;
}

/* normally this is the main() function entry, but if OpenOCD is linked
 * into application, then this fn will not be invoked, but rather that
 * application will have it's own implementation of main(). */
int
openocd_main(int argc, char *argv[])
{
	int ret;

	/* initialize commandline interface */
	struct command_context *cmd_ctx;

	cmd_ctx = setup_command_handler(NULL);

	if(util_init(cmd_ctx) != ERROR_OK
	)
		return EXIT_FAILURE;

	if(ioutil_init(cmd_ctx) != ERROR_OK
	)
		return EXIT_FAILURE;

	LOG_OUTPUT("For bug reports, read\n\t"
	"http://openocd.berlios.de/doc/doxygen/bugs.html"
	"\n");

	command_context_mode(cmd_ctx, COMMAND_CONFIG);
	//command_set_output_handler(cmd_ctx, configuration_output_handler, NULL);

	/* Start the executable meat that can evolve into thread in future. */
	ret = openocd_thread(argc, argv, cmd_ctx);

	unregister_all_commands(cmd_ctx, NULL);

	/* free commandline interface */
	command_done(cmd_ctx);

	adapter_quit();

	return ret;
}
