/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 1999 - 2007, Digium, Inc.
 *
 * Mark Michelson <mmichelson@digium.com>
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
 * \brief globally accessible channel datastores
 * \author Mark Michelson <mmichelson@digium.com>
 */

#ifndef _ASTERISK_GLOBAL_DATASTORE_H
#define _ASTERISK_GLOBAL_DATASTORE_H

#include "asterisk/channel.h"

#define MAX_DIAL_FEATURE_OPTIONS 30

extern const struct ast_datastore_info dialed_interface_info;

extern const struct ast_datastore_info dial_features_info;

struct ast_dialed_interface {
	AST_LIST_ENTRY(ast_dialed_interface) list;
	char interface[1];
};

struct ast_dial_features {
	struct ast_flags features_caller;
	struct ast_flags features_callee;
	char options[MAX_DIAL_FEATURE_OPTIONS];
	int is_caller;
};

#endif
