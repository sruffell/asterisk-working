/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2009, Digium, Inc.
 *
 * David Brooks <dbrooks@digium.com>
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

/*! \file
 *
 * \brief Test AMI hook
 *
 * \author David Brooks <dbrooks@digium.com> based off of code written by Russell Bryant <russell@digium.com>
 *
 * This is simply an example or test module illustrating the ability for a custom module
 * to hook into AMI. Registration for AMI events and sending of AMI actions is shown.
 */

/*** MODULEINFO
	<defaultenabled>no</defaultenabled>
 ***/

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/module.h"
#include "asterisk/cli.h"
#include "asterisk/utils.h"
#include "asterisk/manager.h"

/* The helper function is required by struct manager_custom_hook. See __manager_event for details */
static int amihook_helper(int category, const char *event, char *content)
{
	ast_log(LOG_NOTICE, "AMI Event: \nCategory: %d Event: %s\n%s\n", category, event, content);
	return 0;
}

static struct manager_custom_hook test_hook = {
	.file = __FILE__,
	.helper = &amihook_helper,
};

static int test_send(struct ast_cli_args *a) {
	int res;

	/* Send a test action (core show version) to the AMI */
	res = ast_hook_send_action(&test_hook, "Action: Command\nCommand: core show version\nActionID: 987654321\n");

	return res;
}

static char *handle_cli_amihook_test_send(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	switch (cmd) {
	case CLI_INIT:
		e->command = "amihook send test";
		e->usage = ""
			"Usage: amihook send test"
			"";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	case CLI_HANDLER:
		test_send(a);
		return CLI_SUCCESS;
	}

	return CLI_FAILURE;
}

static struct ast_cli_entry cli_amihook_evt[] = {
	AST_CLI_DEFINE(handle_cli_amihook_test_send, "Test module for AMI hook"),
};

static int unload_module(void)
{
	ast_manager_unregister_hook(&test_hook);
	return ast_cli_unregister_multiple(cli_amihook_evt, ARRAY_LEN(cli_amihook_evt));
}

static int load_module(void)
{
	int res;

	/* Register the hook for AMI events */
	ast_manager_register_hook(&test_hook);

	res = ast_cli_register_multiple(cli_amihook_evt, ARRAY_LEN(cli_amihook_evt));

	return res ? AST_MODULE_LOAD_DECLINE : AST_MODULE_LOAD_SUCCESS;
}

AST_MODULE_INFO_STANDARD(ASTERISK_GPL_KEY, "AMI Hook Test Module");
