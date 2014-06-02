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

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER org_asterisk_core

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./org_asterisk_core_provider.h"

#if !defined(_ORG_ASTERISK_CORE_PROVIDER_H_) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _ORG_ASTERISK_CORE_PROVIDER_H_

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(org_asterisk_core, start, TP_ARGS(), TP_FIELDS())
TRACEPOINT_LOGLEVEL(org_asterisk_core, start, TRACE_INFO)

TRACEPOINT_EVENT(
	org_asterisk_core,
	message,
	TP_ARGS(char *, text),
	TP_FIELDS(
		ctf_string(message, text)
	)
)
TRACEPOINT_LOGLEVEL(org_asterisk_core, message, TRACE_INFO)

#endif /* _TRACEPOINT_UST_ASTERISK_H */

#include <lttng/tracepoint-event.h>
