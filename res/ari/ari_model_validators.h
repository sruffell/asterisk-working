/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2013, Digium, Inc.
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
 * \brief Generated file - Build validators for ARI model objects.
 *
 * In addition to the normal validation functions one would normally expect,
 * each validator has a ast_ari_validate_{id}_fn() companion function that returns
 * the validator's function pointer.
 *
 * The reason for this seamingly useless indirection is the way function
 * pointers interfere with module loading. Asterisk attempts to dlopen() each
 * module using \c RTLD_LAZY in order to read some metadata from the module.
 * Unfortunately, if you take the address of a function, the function has to be
 * resolvable at load time, even if \c RTLD_LAZY is specified. By moving the
 * function-address-taking into this module, we can once again be lazy.
 */

 /*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!!!                               DO NOT EDIT                        !!!!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * This file is generated by a mustache template. Please see the original
 * template in rest-api-templates/ari_model_validators.h.mustache
 */

#ifndef _ASTERISK_ARI_MODEL_H
#define _ASTERISK_ARI_MODEL_H

#include "asterisk/json.h"

/*! @{ */

/*!
 * \brief Validator for native Swagger void.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_void(struct ast_json *json);

/*!
 * \brief Validator for native Swagger object.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_object(struct ast_json *json);

/*!
 * \brief Validator for native Swagger byte.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_byte(struct ast_json *json);

/*!
 * \brief Validator for native Swagger boolean.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_boolean(struct ast_json *json);

/*!
 * \brief Validator for native Swagger int.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_int(struct ast_json *json);

/*!
 * \brief Validator for native Swagger long.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_long(struct ast_json *json);

/*!
 * \brief Validator for native Swagger float.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_float(struct ast_json *json);

/*!
 * \brief Validator for native Swagger double.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_double(struct ast_json *json);

/*!
 * \brief Validator for native Swagger string.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_string(struct ast_json *json);

/*!
 * \brief Validator for native Swagger date.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_date(struct ast_json *json);

/*!
 * \brief Validator for a Swagger List[]/JSON array.
 *
 * \param json JSON object to validate.
 * \param fn Validator to call on every element in the array.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_list(struct ast_json *json, int (*fn)(struct ast_json *));

/*! @} */

/*!
 * \brief Function type for validator functions. Allows for 
 */
typedef int (*ari_validator)(struct ast_json *json);

/*!
 * \brief Validator for AsteriskInfo.
 *
 * Asterisk system information
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_asterisk_info(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_asterisk_info().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_asterisk_info_fn(void);

/*!
 * \brief Validator for BuildInfo.
 *
 * Info about how Asterisk was built
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_build_info(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_build_info().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_build_info_fn(void);

/*!
 * \brief Validator for ConfigInfo.
 *
 * Info about Asterisk configuration
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_config_info(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_config_info().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_config_info_fn(void);

/*!
 * \brief Validator for SetId.
 *
 * Effective user/group id
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_set_id(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_set_id().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_set_id_fn(void);

/*!
 * \brief Validator for StatusInfo.
 *
 * Info about Asterisk status
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_status_info(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_status_info().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_status_info_fn(void);

/*!
 * \brief Validator for SystemInfo.
 *
 * Info about Asterisk
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_system_info(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_system_info().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_system_info_fn(void);

/*!
 * \brief Validator for Variable.
 *
 * The value of a channel variable
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_variable(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_variable().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_variable_fn(void);

/*!
 * \brief Validator for Endpoint.
 *
 * An external device that may offer/accept calls to/from Asterisk.
 *
 * Unlike most resources, which have a single unique identifier, an endpoint is uniquely identified by the technology/resource pair.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_endpoint(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_endpoint().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_endpoint_fn(void);

/*!
 * \brief Validator for TextMessage.
 *
 * A text message.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_text_message(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_text_message().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_text_message_fn(void);

/*!
 * \brief Validator for TextMessageVariable.
 *
 * A key/value pair variable in a text message.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_text_message_variable(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_text_message_variable().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_text_message_variable_fn(void);

/*!
 * \brief Validator for CallerID.
 *
 * Caller identification
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_caller_id(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_caller_id().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_caller_id_fn(void);

/*!
 * \brief Validator for Channel.
 *
 * A specific communication connection between Asterisk and an Endpoint.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_fn(void);

/*!
 * \brief Validator for Dialed.
 *
 * Dialed channel information.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_dialed(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_dialed().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_dialed_fn(void);

/*!
 * \brief Validator for DialplanCEP.
 *
 * Dialplan location (context/extension/priority)
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_dialplan_cep(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_dialplan_cep().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_dialplan_cep_fn(void);

/*!
 * \brief Validator for Bridge.
 *
 * The merging of media from one or more channels.
 *
 * Everyone on the bridge receives the same audio.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_bridge(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_bridge().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_bridge_fn(void);

/*!
 * \brief Validator for LiveRecording.
 *
 * A recording that is in progress
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_live_recording(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_live_recording().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_live_recording_fn(void);

/*!
 * \brief Validator for StoredRecording.
 *
 * A past recording that may be played back.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_stored_recording(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_stored_recording().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_stored_recording_fn(void);

/*!
 * \brief Validator for FormatLangPair.
 *
 * Identifies the format and language of a sound file
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_format_lang_pair(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_format_lang_pair().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_format_lang_pair_fn(void);

/*!
 * \brief Validator for Sound.
 *
 * A media file that may be played back.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_sound(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_sound().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_sound_fn(void);

/*!
 * \brief Validator for Playback.
 *
 * Object representing the playback of media to a channel
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_playback(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_playback().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_playback_fn(void);

/*!
 * \brief Validator for DeviceState.
 *
 * Represents the state of a device.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_device_state(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_device_state().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_device_state_fn(void);

/*!
 * \brief Validator for Mailbox.
 *
 * Represents the state of a mailbox.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_mailbox(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_mailbox().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_mailbox_fn(void);

/*!
 * \brief Validator for ApplicationReplaced.
 *
 * Notification that another WebSocket has taken over for an application.
 *
 * An application may only be subscribed to by a single WebSocket at a time. If multiple WebSockets attempt to subscribe to the same application, the newer WebSocket wins, and the older one receives this event.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_application_replaced(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_application_replaced().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_application_replaced_fn(void);

/*!
 * \brief Validator for BridgeAttendedTransfer.
 *
 * Notification that an attended transfer has occurred.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_bridge_attended_transfer(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_bridge_attended_transfer().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_bridge_attended_transfer_fn(void);

/*!
 * \brief Validator for BridgeBlindTransfer.
 *
 * Notification that a blind transfer has occurred.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_bridge_blind_transfer(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_bridge_blind_transfer().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_bridge_blind_transfer_fn(void);

/*!
 * \brief Validator for BridgeCreated.
 *
 * Notification that a bridge has been created.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_bridge_created(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_bridge_created().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_bridge_created_fn(void);

/*!
 * \brief Validator for BridgeDestroyed.
 *
 * Notification that a bridge has been destroyed.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_bridge_destroyed(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_bridge_destroyed().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_bridge_destroyed_fn(void);

/*!
 * \brief Validator for BridgeMerged.
 *
 * Notification that one bridge has merged into another.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_bridge_merged(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_bridge_merged().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_bridge_merged_fn(void);

/*!
 * \brief Validator for ChannelCallerId.
 *
 * Channel changed Caller ID.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_caller_id(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_caller_id().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_caller_id_fn(void);

/*!
 * \brief Validator for ChannelCreated.
 *
 * Notification that a channel has been created.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_created(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_created().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_created_fn(void);

/*!
 * \brief Validator for ChannelDestroyed.
 *
 * Notification that a channel has been destroyed.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_destroyed(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_destroyed().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_destroyed_fn(void);

/*!
 * \brief Validator for ChannelDialplan.
 *
 * Channel changed location in the dialplan.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_dialplan(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_dialplan().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_dialplan_fn(void);

/*!
 * \brief Validator for ChannelDtmfReceived.
 *
 * DTMF received on a channel.
 *
 * This event is sent when the DTMF ends. There is no notification about the start of DTMF
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_dtmf_received(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_dtmf_received().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_dtmf_received_fn(void);

/*!
 * \brief Validator for ChannelEnteredBridge.
 *
 * Notification that a channel has entered a bridge.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_entered_bridge(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_entered_bridge().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_entered_bridge_fn(void);

/*!
 * \brief Validator for ChannelHangupRequest.
 *
 * A hangup was requested on the channel.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_hangup_request(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_hangup_request().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_hangup_request_fn(void);

/*!
 * \brief Validator for ChannelLeftBridge.
 *
 * Notification that a channel has left a bridge.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_left_bridge(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_left_bridge().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_left_bridge_fn(void);

/*!
 * \brief Validator for ChannelStateChange.
 *
 * Notification of a channel's state change.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_state_change(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_state_change().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_state_change_fn(void);

/*!
 * \brief Validator for ChannelTalkingFinished.
 *
 * Talking is no longer detected on the channel.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_talking_finished(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_talking_finished().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_talking_finished_fn(void);

/*!
 * \brief Validator for ChannelTalkingStarted.
 *
 * Talking was detected on the channel.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_talking_started(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_talking_started().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_talking_started_fn(void);

/*!
 * \brief Validator for ChannelUserevent.
 *
 * User-generated event with additional user-defined fields in the object.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_userevent(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_userevent().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_userevent_fn(void);

/*!
 * \brief Validator for ChannelVarset.
 *
 * Channel variable changed.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_channel_varset(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_channel_varset().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_channel_varset_fn(void);

/*!
 * \brief Validator for DeviceStateChanged.
 *
 * Notification that a device state has changed.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_device_state_changed(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_device_state_changed().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_device_state_changed_fn(void);

/*!
 * \brief Validator for Dial.
 *
 * Dialing state has changed.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_dial(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_dial().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_dial_fn(void);

/*!
 * \brief Validator for EndpointStateChange.
 *
 * Endpoint state changed.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_endpoint_state_change(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_endpoint_state_change().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_endpoint_state_change_fn(void);

/*!
 * \brief Validator for Event.
 *
 * Base type for asynchronous events from Asterisk.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_event(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_event().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_event_fn(void);

/*!
 * \brief Validator for Message.
 *
 * Base type for errors and events
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_message(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_message().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_message_fn(void);

/*!
 * \brief Validator for MissingParams.
 *
 * Error event sent when required params are missing.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_missing_params(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_missing_params().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_missing_params_fn(void);

/*!
 * \brief Validator for PlaybackFinished.
 *
 * Event showing the completion of a media playback operation.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_playback_finished(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_playback_finished().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_playback_finished_fn(void);

/*!
 * \brief Validator for PlaybackStarted.
 *
 * Event showing the start of a media playback operation.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_playback_started(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_playback_started().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_playback_started_fn(void);

/*!
 * \brief Validator for RecordingFailed.
 *
 * Event showing failure of a recording operation.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_recording_failed(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_recording_failed().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_recording_failed_fn(void);

/*!
 * \brief Validator for RecordingFinished.
 *
 * Event showing the completion of a recording operation.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_recording_finished(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_recording_finished().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_recording_finished_fn(void);

/*!
 * \brief Validator for RecordingStarted.
 *
 * Event showing the start of a recording operation.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_recording_started(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_recording_started().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_recording_started_fn(void);

/*!
 * \brief Validator for StasisEnd.
 *
 * Notification that a channel has left a Stasis application.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_stasis_end(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_stasis_end().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_stasis_end_fn(void);

/*!
 * \brief Validator for StasisStart.
 *
 * Notification that a channel has entered a Stasis application.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_stasis_start(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_stasis_start().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_stasis_start_fn(void);

/*!
 * \brief Validator for TextMessageReceived.
 *
 * A text message was received from an endpoint.
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_text_message_received(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_text_message_received().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_text_message_received_fn(void);

/*!
 * \brief Validator for Application.
 *
 * Details of a Stasis application
 *
 * \param json JSON object to validate.
 * \returns True (non-zero) if valid.
 * \returns False (zero) if invalid.
 */
int ast_ari_validate_application(struct ast_json *json);

/*!
 * \brief Function pointer to ast_ari_validate_application().
 *
 * See \ref ast_ari_model_validators.h for more details.
 */
ari_validator ast_ari_validate_application_fn(void);

/*
 * JSON models
 *
 * AsteriskInfo
 * - build: BuildInfo
 * - config: ConfigInfo
 * - status: StatusInfo
 * - system: SystemInfo
 * BuildInfo
 * - date: string (required)
 * - kernel: string (required)
 * - machine: string (required)
 * - options: string (required)
 * - os: string (required)
 * - user: string (required)
 * ConfigInfo
 * - default_language: string (required)
 * - max_channels: int
 * - max_load: double
 * - max_open_files: int
 * - name: string (required)
 * - setid: SetId (required)
 * SetId
 * - group: string (required)
 * - user: string (required)
 * StatusInfo
 * - last_reload_time: Date (required)
 * - startup_time: Date (required)
 * SystemInfo
 * - entity_id: string (required)
 * - version: string (required)
 * Variable
 * - value: string (required)
 * Endpoint
 * - channel_ids: List[string] (required)
 * - resource: string (required)
 * - state: string
 * - technology: string (required)
 * TextMessage
 * - body: string (required)
 * - from: string (required)
 * - to: string (required)
 * - variables: List[TextMessageVariable]
 * TextMessageVariable
 * - key: string (required)
 * - value: string (required)
 * CallerID
 * - name: string (required)
 * - number: string (required)
 * Channel
 * - accountcode: string (required)
 * - caller: CallerID (required)
 * - connected: CallerID (required)
 * - creationtime: Date (required)
 * - dialplan: DialplanCEP (required)
 * - id: string (required)
 * - name: string (required)
 * - state: string (required)
 * Dialed
 * DialplanCEP
 * - context: string (required)
 * - exten: string (required)
 * - priority: long (required)
 * Bridge
 * - bridge_class: string (required)
 * - bridge_type: string (required)
 * - channels: List[string] (required)
 * - creator: string (required)
 * - id: string (required)
 * - name: string (required)
 * - technology: string (required)
 * LiveRecording
 * - cause: string
 * - duration: int
 * - format: string (required)
 * - name: string (required)
 * - silence_duration: int
 * - state: string (required)
 * - talking_duration: int
 * - target_uri: string (required)
 * StoredRecording
 * - format: string (required)
 * - name: string (required)
 * FormatLangPair
 * - format: string (required)
 * - language: string (required)
 * Sound
 * - formats: List[FormatLangPair] (required)
 * - id: string (required)
 * - text: string
 * Playback
 * - id: string (required)
 * - language: string
 * - media_uri: string (required)
 * - state: string (required)
 * - target_uri: string (required)
 * DeviceState
 * - name: string (required)
 * - state: string (required)
 * Mailbox
 * - name: string (required)
 * - new_messages: int (required)
 * - old_messages: int (required)
 * ApplicationReplaced
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * BridgeAttendedTransfer
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - destination_application: string
 * - destination_bridge: string
 * - destination_link_first_leg: Channel
 * - destination_link_second_leg: Channel
 * - destination_threeway_bridge: Bridge
 * - destination_threeway_channel: Channel
 * - destination_type: string (required)
 * - is_external: boolean (required)
 * - replace_channel: Channel
 * - result: string (required)
 * - transfer_target: Channel
 * - transferee: Channel
 * - transferer_first_leg: Channel (required)
 * - transferer_first_leg_bridge: Bridge
 * - transferer_second_leg: Channel (required)
 * - transferer_second_leg_bridge: Bridge
 * BridgeBlindTransfer
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - bridge: Bridge
 * - channel: Channel (required)
 * - context: string (required)
 * - exten: string (required)
 * - is_external: boolean (required)
 * - replace_channel: Channel
 * - result: string (required)
 * - transferee: Channel
 * BridgeCreated
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - bridge: Bridge (required)
 * BridgeDestroyed
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - bridge: Bridge (required)
 * BridgeMerged
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - bridge: Bridge (required)
 * - bridge_from: Bridge (required)
 * ChannelCallerId
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - caller_presentation: int (required)
 * - caller_presentation_txt: string (required)
 * - channel: Channel (required)
 * ChannelCreated
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - channel: Channel (required)
 * ChannelDestroyed
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - cause: int (required)
 * - cause_txt: string (required)
 * - channel: Channel (required)
 * ChannelDialplan
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - channel: Channel (required)
 * - dialplan_app: string (required)
 * - dialplan_app_data: string (required)
 * ChannelDtmfReceived
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - channel: Channel (required)
 * - digit: string (required)
 * - duration_ms: int (required)
 * ChannelEnteredBridge
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - bridge: Bridge (required)
 * - channel: Channel
 * ChannelHangupRequest
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - cause: int
 * - channel: Channel (required)
 * - soft: boolean
 * ChannelLeftBridge
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - bridge: Bridge (required)
 * - channel: Channel (required)
 * ChannelStateChange
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - channel: Channel (required)
 * ChannelTalkingFinished
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - channel: Channel (required)
 * - duration: int (required)
 * ChannelTalkingStarted
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - channel: Channel (required)
 * ChannelUserevent
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - bridge: Bridge
 * - channel: Channel
 * - endpoint: Endpoint
 * - eventname: string (required)
 * - userevent: object (required)
 * ChannelVarset
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - channel: Channel
 * - value: string (required)
 * - variable: string (required)
 * DeviceStateChanged
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - device_state: DeviceState (required)
 * Dial
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - caller: Channel
 * - dialstatus: string (required)
 * - dialstring: string
 * - forward: string
 * - forwarded: Channel
 * - peer: Channel (required)
 * EndpointStateChange
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - endpoint: Endpoint (required)
 * Event
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * Message
 * - type: string (required)
 * MissingParams
 * - type: string (required)
 * - params: List[string] (required)
 * PlaybackFinished
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - playback: Playback (required)
 * PlaybackStarted
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - playback: Playback (required)
 * RecordingFailed
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - recording: LiveRecording (required)
 * RecordingFinished
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - recording: LiveRecording (required)
 * RecordingStarted
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - recording: LiveRecording (required)
 * StasisEnd
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - channel: Channel (required)
 * StasisStart
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - args: List[string] (required)
 * - channel: Channel (required)
 * - replace_channel: Channel
 * TextMessageReceived
 * - type: string (required)
 * - application: string (required)
 * - timestamp: Date
 * - endpoint: Endpoint
 * - message: TextMessage (required)
 * Application
 * - bridge_ids: List[string] (required)
 * - channel_ids: List[string] (required)
 * - device_names: List[string] (required)
 * - endpoint_ids: List[string] (required)
 * - name: string (required)
 */

#endif /* _ASTERISK_ARI_MODEL_H */
