/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2012 - 2013, Digium, Inc.
 *
 * David M. Lee, II <dlee@digium.com>
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

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!!!                               DO NOT EDIT                        !!!!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * This file is generated by a mustache template. Please see the original
 * template in rest-api-templates/res_stasis_http_resource.c.mustache
 */

/*! \file
 *
 * \brief Playback control resources
 *
 * \author David M. Lee, II <dlee@digium.com>
 */

/*** MODULEINFO
	<depend type="module">res_stasis_http</depend>
	<depend type="module">res_stasis</depend>
	<support_level>core</support_level>
 ***/

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/module.h"
#include "asterisk/stasis_app.h"
#include "stasis_http/resource_playback.h"
#if defined(AST_DEVMODE)
#include "stasis_http/ari_model_validators.h"
#endif

/*!
 * \brief Parameter parsing callback for /playback/{playbackId}.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void stasis_http_get_playback_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct stasis_http_response *response)
{
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	struct ast_get_playback_args args = {};
	struct ast_variable *i;

	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "playbackId") == 0) {
			args.playback_id = (i->value);
		} else
		{}
	}
	stasis_http_get_playback(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 500: /* Internal server error */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ari_validate_playback(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /playback/{playbackId}\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /playback/{playbackId}\n");
		stasis_http_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */
}
/*!
 * \brief Parameter parsing callback for /playback/{playbackId}.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void stasis_http_stop_playback_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct stasis_http_response *response)
{
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	struct ast_stop_playback_args args = {};
	struct ast_variable *i;

	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "playbackId") == 0) {
			args.playback_id = (i->value);
		} else
		{}
	}
	stasis_http_stop_playback(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 500: /* Internal server error */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ari_validate_playback(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /playback/{playbackId}\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /playback/{playbackId}\n");
		stasis_http_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */
}
/*!
 * \brief Parameter parsing callback for /playback/{playbackId}/control.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void stasis_http_control_playback_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct stasis_http_response *response)
{
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	struct ast_control_playback_args args = {};
	struct ast_variable *i;

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "operation") == 0) {
			args.operation = (i->value);
		} else
		{}
	}
	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "playbackId") == 0) {
			args.playback_id = (i->value);
		} else
		{}
	}
	stasis_http_control_playback(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 500: /* Internal server error */
	case 400: /* The provided operation parameter was invalid */
	case 404: /* The playback cannot be found */
	case 409: /* The operation cannot be performed in the playback's current state */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ari_validate_playback(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /playback/{playbackId}/control\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /playback/{playbackId}/control\n");
		stasis_http_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */
}

/*! \brief REST handler for /api-docs/playback.{format} */
static struct stasis_rest_handlers playback_playbackId_control = {
	.path_segment = "control",
	.callbacks = {
		[AST_HTTP_POST] = stasis_http_control_playback_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/playback.{format} */
static struct stasis_rest_handlers playback_playbackId = {
	.path_segment = "playbackId",
	.is_wildcard = 1,
	.callbacks = {
		[AST_HTTP_GET] = stasis_http_get_playback_cb,
		[AST_HTTP_DELETE] = stasis_http_stop_playback_cb,
	},
	.num_children = 1,
	.children = { &playback_playbackId_control, }
};
/*! \brief REST handler for /api-docs/playback.{format} */
static struct stasis_rest_handlers playback = {
	.path_segment = "playback",
	.callbacks = {
	},
	.num_children = 1,
	.children = { &playback_playbackId, }
};

static int load_module(void)
{
	int res = 0;
	stasis_app_ref();
	res |= stasis_http_add_handler(&playback);
	return res;
}

static int unload_module(void)
{
	stasis_http_remove_handler(&playback);
	stasis_app_unref();
	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "RESTful API module - Playback control resources",
	.load = load_module,
	.unload = unload_module,
	.nonoptreq = "res_stasis_http,res_stasis",
	);