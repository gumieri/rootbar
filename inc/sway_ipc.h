/*
 *  Copyright (C) 2019 Scoopta
 *  This file is part of Root Bar
 *  Root Bar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Root Bar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Root Bar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SWAY_IPC_H
#define SWAY_IPC_H

#include <map.h>
#include <utils.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

enum sway_ipc_event {
	SWAY_IPC_EVENT_WORKSPACE,
	SWAY_IPC_EVENT_OUTPUT,
	SWAY_IPC_EVENT_MODE,
	SWAY_IPC_EVENT_WINDOW,
	SWAY_IPC_EVENT_BARCONFIG_UPDATE,
	SWAY_IPC_EVENT_BINDING,
	SWAY_IPC_EVENT_SHUTDOWN,
	SWAY_IPC_EVENT_TICK
};

enum sway_ipc_message {
	SWAY_IPC_MESSAGE_RUN_COMMAND,
	SWAY_IPC_MESSAGE_GET_WORKSPACES,
	SWAY_IPC_MESSAGE_SUBSCRIBE,
	SWAY_IPC_MESSAGE_GET_OUTPUTS,
	SWAY_IPC_MESSAGE_GET_TREE,
	SWAY_IPC_MESSAGE_GET_MARKS,
	SWAY_IPC_MESSAGE_GET_BAR_CONFIG,
	SWAY_IPC_MESSAGE_GET_VERSION,
	SWAY_IPC_MESSAGE_GET_BINDING_MODES,
	SWAY_IPC_MESSAGE_GET_CONFIG,
	SWAY_IPC_MESSAGE_SEND_TICK
};

enum sway_ipc_reply {
	SWAY_IPC_REPLY_COMMAND,
	SWAY_IPC_REPLY_WORKSPACES,
	SWAY_IPC_REPLY_SUBSCRIBE,
	SWAY_IPC_REPLY_OUTPUTS,
	SWAY_IPC_REPLY_TREE,
	SWAY_IPC_REPLY_MARKS,
	SWAY_IPC_REPLY_BAR_CONFIG,
	SWAY_IPC_REPLY_VERSION,
	SWAY_IPC_REPLY_BINDING_MODES,
	SWAY_IPC_REPLY_GET_CONFIG,
	SWAY_IPC_REPLY_TICK
};

struct sway_ipc* sway_ipc_init();

void sway_ipc_subscribe(struct sway_ipc* this, enum sway_ipc_event event, void (*handler)(void* data, const char* str), void* data);

char* sway_ipc_send_message(struct sway_ipc* this, enum sway_ipc_message message, const char* payload, enum sway_ipc_reply expected_reply);

#endif
