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

/*! \file
 *
 * \brief Generated file - declares stubs to be implemented in
 * res/stasis_http/resource_channels.c
 *
 * Channel resources
 *
 * \author David M. Lee, II <dlee@digium.com>
 */

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!!!                               DO NOT EDIT                        !!!!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * This file is generated by a mustache template. Please see the original
 * template in rest-api-templates/stasis_http_resource.h.mustache
 */

#ifndef _ASTERISK_RESOURCE_CHANNELS_H
#define _ASTERISK_RESOURCE_CHANNELS_H

#include "asterisk/stasis_http.h"

/*! \brief Argument struct for stasis_http_get_channels() */
struct ast_get_channels_args {
};
/*!
 * \brief List active channels.
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_get_channels(struct ast_variable *headers, struct ast_get_channels_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_originate() */
struct ast_originate_args {
	/*! \brief Endpoint to call. If not specified, originate is routed via dialplan */
	const char *endpoint;
	/*! \brief Extension to dial */
	const char *extension;
	/*! \brief When routing via dialplan, the context use. If omitted, uses 'default' */
	const char *context;
};
/*!
 * \brief Create a new channel (originate).
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_originate(struct ast_variable *headers, struct ast_originate_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_get_channel() */
struct ast_get_channel_args {
	/*! \brief Channel's id */
	const char *channel_id;
};
/*!
 * \brief Channel details.
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_get_channel(struct ast_variable *headers, struct ast_get_channel_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_delete_channel() */
struct ast_delete_channel_args {
	/*! \brief Channel's id */
	const char *channel_id;
};
/*!
 * \brief Delete (i.e. hangup) a channel.
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_delete_channel(struct ast_variable *headers, struct ast_delete_channel_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_dial() */
struct ast_dial_args {
	/*! \brief Channel's id */
	const char *channel_id;
	/*! \brief Endpoint to call. If not specified, dial is routed via dialplan */
	const char *endpoint;
	/*! \brief Extension to dial */
	const char *extension;
	/*! \brief When routing via dialplan, the context use. If omitted, uses 'default' */
	const char *context;
};
/*!
 * \brief Create a new channel (originate) and bridge to this channel.
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_dial(struct ast_variable *headers, struct ast_dial_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_continue_in_dialplan() */
struct ast_continue_in_dialplan_args {
	/*! \brief Channel's id */
	const char *channel_id;
};
/*!
 * \brief Exit application; continue execution in the dialplan.
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_continue_in_dialplan(struct ast_variable *headers, struct ast_continue_in_dialplan_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_answer_channel() */
struct ast_answer_channel_args {
	/*! \brief Channel's id */
	const char *channel_id;
};
/*!
 * \brief Answer a channel.
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_answer_channel(struct ast_variable *headers, struct ast_answer_channel_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_mute_channel() */
struct ast_mute_channel_args {
	/*! \brief Channel's id */
	const char *channel_id;
	/*! \brief Direction in which to mute audio */
	const char *direction;
};
/*!
 * \brief Mute a channel.
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_mute_channel(struct ast_variable *headers, struct ast_mute_channel_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_unmute_channel() */
struct ast_unmute_channel_args {
	/*! \brief Channel's id */
	const char *channel_id;
	/*! \brief Direction in which to unmute audio */
	const char *direction;
};
/*!
 * \brief Unmute a channel.
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_unmute_channel(struct ast_variable *headers, struct ast_unmute_channel_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_hold_channel() */
struct ast_hold_channel_args {
	/*! \brief Channel's id */
	const char *channel_id;
};
/*!
 * \brief Hold a channel.
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_hold_channel(struct ast_variable *headers, struct ast_hold_channel_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_unhold_channel() */
struct ast_unhold_channel_args {
	/*! \brief Channel's id */
	const char *channel_id;
};
/*!
 * \brief Remove a channel from hold.
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_unhold_channel(struct ast_variable *headers, struct ast_unhold_channel_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_play_on_channel() */
struct ast_play_on_channel_args {
	/*! \brief Channel's id */
	const char *channel_id;
	/*! \brief Media's URI to play. */
	const char *media;
	/*! \brief For sounds, selects language for sound. */
	const char *lang;
	/*! \brief Number of media to skip before playing. */
	int offsetms;
	/*! \brief Number of milliseconds to skip for forward/reverse operations. */
	int skipms;
};
/*!
 * \brief Start playback of media.
 *
 * The media URI may be any of a number of URI's. You may use http: and https: URI's, as well as sound: and recording: URI's. This operation creates a playback resource that can be used to control the playback of media (pause, rewind, fast forward, etc.)
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_play_on_channel(struct ast_variable *headers, struct ast_play_on_channel_args *args, struct stasis_http_response *response);
/*! \brief Argument struct for stasis_http_record_channel() */
struct ast_record_channel_args {
	/*! \brief Channel's id */
	const char *channel_id;
	/*! \brief Recording's filename */
	const char *name;
	/*! \brief Format to encode audio in */
	const char *format;
	/*! \brief Maximum duration of the recording, in seconds. 0 for no limit */
	int max_duration_seconds;
	/*! \brief Maximum duration of silence, in seconds. 0 for no limit */
	int max_silence_seconds;
	/*! \brief If true, and recording already exists, append to recording */
	int append;
	/*! \brief Play beep when recording begins */
	int beep;
	/*! \brief DTMF input to terminate recording */
	const char *terminate_on;
};
/*!
 * \brief Start a recording.
 *
 * Record audio from a channel. Note that this will not capture audio sent to the channel. The bridge itself has a record feature if that's what you want.
 *
 * \param headers HTTP headers
 * \param args Swagger parameters
 * \param[out] response HTTP response
 */
void stasis_http_record_channel(struct ast_variable *headers, struct ast_record_channel_args *args, struct stasis_http_response *response);

#endif /* _ASTERISK_RESOURCE_CHANNELS_H */
