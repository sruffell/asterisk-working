/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 1999 - 2014, Digium, Inc.
 *
 * Mark Spencer <markster@digium.com>
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
 * \brief Asterisk Linux Trace Toolkit Tracepoint providers
 */

#include "asterisk/autoconfig.h"

#ifdef HAVE_LTTNG_UST

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER org_asterisk_core

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./org_asterisk_core_provider.h"

#if !defined(_ORG_ASTERISK_CORE_PROVIDER_H_) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _ORG_ASTERISK_CORE_PROVIDER_H_

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(org_asterisk_core, start,
	TP_ARGS(const char *, version), 
	TP_FIELDS(
		ctf_string(version, version)
	)
)
TRACEPOINT_LOGLEVEL(org_asterisk_core, start, TRACE_INFO)

TRACEPOINT_EVENT(org_asterisk_core, dynamic_loader_start,
	TP_ARGS(int, load_count),
	TP_FIELDS(
		ctf_integer(int, module_to_load, load_count)
	)
)
TRACEPOINT_LOGLEVEL(org_asterisk_core, dynamic_loader_start, TRACE_INFO)

TRACEPOINT_EVENT(org_asterisk_core, dynamic_loader_loadstart,
	TP_ARGS(const char *, text),
	TP_FIELDS(
		ctf_string(module_name, text)
	)
)
TRACEPOINT_LOGLEVEL(org_asterisk_core, dynamic_loader_loadstart, TRACE_INFO)

TRACEPOINT_EVENT(org_asterisk_core, dynamic_loader_loadcomplete,
	TP_ARGS(const char *, text, int, result_int),
	TP_FIELDS(
		ctf_string(module_name, text)
		ctf_integer(int, result, result_int)
	)
)
TRACEPOINT_LOGLEVEL(org_asterisk_core, dynamic_loader_loadcomplete, TRACE_INFO)

TRACEPOINT_EVENT(org_asterisk_core, forked, TP_ARGS(), TP_FIELDS())
TRACEPOINT_LOGLEVEL(org_asterisk_core, forked, TRACE_INFO)

TRACEPOINT_EVENT(
	org_asterisk_core,
	message,
	TP_ARGS(char *, text),
	TP_FIELDS(
		ctf_string(message, text)
	)
)
TRACEPOINT_LOGLEVEL(org_asterisk_core, message, TRACE_INFO)

TRACEPOINT_EVENT(
	org_asterisk_core,
	logger_print_normal,
	TP_ARGS(const char *, file, int, line,
		const char *, function, const char *, message ),
	TP_FIELDS(
		ctf_string(file, file)
		ctf_integer(int, line, line)
		ctf_string(function, function)
		ctf_string(message, message)
	)
)
TRACEPOINT_LOGLEVEL(org_asterisk_core, logger_print_normal, TRACE_DEBUG)

TRACEPOINT_EVENT(
	org_asterisk_core,
	channel_register,
	TP_ARGS(const char *, type, const char *, description),
	TP_FIELDS(
		ctf_string(type, type)
		ctf_string(description, description)
	)
)
TRACEPOINT_LOGLEVEL(org_asterisk_core, channel_register, TRACE_INFO)

TRACEPOINT_EVENT(
	org_asterisk_core,
	channel_unregister,
	TP_ARGS(const char *, type, const char *, description),
	TP_FIELDS(
		ctf_string(type, type)
		ctf_string(description, description)
	)
)
TRACEPOINT_LOGLEVEL(org_asterisk_core, channel_unregister, TRACE_INFO)

TRACEPOINT_EVENT(
	org_asterisk_core,
	channel_create,
	TP_ARGS(const char *, file, int, line, const char *, function, const char *, channel_name),
	TP_FIELDS(
		ctf_string(file, file)
		ctf_integer(int, line, line)
		ctf_string(function, function)
		ctf_string(channel_name, channel_name)
	)
)
TRACEPOINT_LOGLEVEL(org_asterisk_core, channel_create, TRACE_INFO)

TRACEPOINT_EVENT(
	org_asterisk_core,
	channel_destroy,
	TP_ARGS(const char *, channel_name),
	TP_FIELDS(
		ctf_string(channel_name, channel_name)
	)
)
TRACEPOINT_LOGLEVEL(org_asterisk_core, channel_destroy, TRACE_INFO)
#endif /* _TRACEPOINT_UST_ASTERISK_H */

#include <lttng/tracepoint-event.h>

#else

#define tracepoint(...) do { } while(0)

#endif
