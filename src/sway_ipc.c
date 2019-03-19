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

#include <sway_ipc.h>

static const char* MAGIC = "i3-ipc";
static char* event_names[] = {"barconfig_update", "binding", "mode", "output", "shutdown", "tick", "window", "workspace"};

struct sway_ipc {
	int32_t event_sock, msg_sock;
	struct map* events;
	bool stop_events;
	pthread_cond_t _stop_events;
	pthread_mutex_t mutex;
};

static void* poll(void* data) {
	struct sway_ipc* this = data;
	size_t magic_s = strlen(MAGIC);
	while(true) {
		size_t buff_s = 8 + magic_s;
		void* buffer = malloc(buff_s);
		recv(this->event_sock, buffer, buff_s, 0);
		if(strncmp(buffer, MAGIC, magic_s) == 0) {
			uint32_t* arr = (uint32_t*) (buffer + magic_s);
			uint32_t length = arr[0];
			uint32_t type = arr[1];
			if(length > 0) {
				free(buffer);
				buffer = malloc(length);
				recv(this->event_sock, buffer, length, 0);
			}
			if(type >> 31 == 1) {
				void (*handle)(char* str) = map_get(this->events, event_names[type & 0xF]);
				handle(buffer);
			}
			free(buffer);
			while(this->stop_events) {
				pthread_mutex_lock(&this->mutex);
				pthread_cond_wait(&this->_stop_events, &this->mutex);
				pthread_mutex_unlock(&this->mutex);
			}
		}
	}
	return NULL;
}

static void stop_events(struct sway_ipc* this) {
	this->stop_events = true;
	size_t magic_s = strlen(MAGIC);
	size_t buff_s = 8 + magic_s;
	void* buffer = malloc(buff_s);
	strcpy(buffer, MAGIC);
	uint32_t* arr = (uint32_t*) (buffer + magic_s);
	arr[0] = 0;
	arr[1] = SWAY_IPC_MESSAGE_SEND_TICK;
	write(this->event_sock, buffer, buff_s);
	free(buffer);
	utils_sleep_millis(100);
}

static void start_events(struct sway_ipc* this) {
	this->stop_events = false;
	pthread_mutex_lock(&this->mutex);
	pthread_cond_broadcast(&this->_stop_events);
	pthread_mutex_unlock(&this->mutex);
}

void sway_ipc_subscribe(struct sway_ipc* this, enum sway_ipc_event event, void (*handler)(char* str)) {
	stop_events(this);
	size_t magic_s = strlen(MAGIC);
	struct json_object* arr = json_object_new_array();
	json_object_array_add(arr, json_object_new_string(event_names[event]));
	const char* payload = json_object_to_json_string(arr);
	size_t payload_s = strlen(payload);
	size_t buff_s = 8 + magic_s + payload_s;
	void* buffer = malloc(buff_s);
	strcpy(buffer, MAGIC);
	uint32_t* int_buff = (uint32_t*) (buffer + magic_s);
	int_buff[0] = payload_s;
	int_buff[1] = SWAY_IPC_MESSAGE_SUBSCRIBE;
	memcpy(buffer + 8 + magic_s, payload, payload_s);
	json_object_put(arr);
	write(this->event_sock, buffer, buff_s);
	free(buffer);

	buff_s = 8 + magic_s;
	buffer = malloc(buff_s);
	recv(this->event_sock, buffer, buff_s, 0);
	if(strncmp(buffer, MAGIC, magic_s) != 0) {
		fprintf(stderr, "Invalid magic\n");
		free(buffer);
		return;
	}
	int_buff = (uint32_t*) (buffer + magic_s);
	uint32_t length = int_buff[0];
	uint32_t type = int_buff[1];
	free(buffer);
	if(type != SWAY_IPC_REPLY_SUBSCRIBE) {
		fprintf(stderr, "Invalid reply\n");
		return;
	}
	if(length == 0) {
		fprintf(stderr, "Payload length of 0");
		return;
	}
	buffer = malloc(length);
	recv(this->event_sock, buffer, buff_s, 0);
	struct json_object* json = json_tokener_parse(buffer);
	struct json_object* success = json_object_object_get(json, "success");
	if(!json_object_get_boolean(success)) {
		fprintf(stderr, "Failed to subscribe to event\n");
		json_object_put(json);
		free(buffer);
		return;
	}
	json_object_put(json);
	free(buffer);
	map_put(this->events, event_names[event], handler);
	start_events(this);
}

char* sway_ipc_send_message(struct sway_ipc* this, enum sway_ipc_message message, const char* payload, enum sway_ipc_reply expected_reply) {
	if(payload == NULL) {
		payload = "";
	}
	size_t magic_s = strlen(MAGIC);
	size_t payload_s = strlen(payload);
	size_t buff_s = 8 + magic_s + payload_s;
	void* buffer = malloc(buff_s);
	memcpy(buffer, MAGIC, magic_s);
	uint32_t* int_buff = (uint32_t*) (buffer + magic_s);
	int_buff[0] = payload_s;
	int_buff[1] = message;
	memcpy(buffer + magic_s + 8, payload, payload_s);
	write(this->msg_sock, buffer, buff_s);
	free(buffer);
	buff_s = 8 + magic_s;
	buffer = malloc(buff_s);
	recv(this->msg_sock, buffer, buff_s, 0);
	if(strncmp(buffer, MAGIC, magic_s) != 0) {
		fprintf(stderr, "Invalid magic\n");
		free(buffer);
		return NULL;
	}
	int_buff = (uint32_t*) (buffer + magic_s);
	uint32_t length = int_buff[0];
	uint32_t type = int_buff[1];
	if(length > 0) {
		free(buffer);
		buffer = malloc(length);
		recv(this->msg_sock, buffer, length, 0);
	}
	if(type != expected_reply) {
		free(buffer);
		fprintf(stderr, "Unexpected reply\n");
		return NULL;
	}
	return buffer;
}

struct sway_ipc* sway_ipc_init() {
	struct sway_ipc* this = malloc(sizeof(struct sway_ipc));
	this->event_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	this->msg_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	struct sockaddr_un addr = {
		.sun_family = AF_UNIX,
	};
	strncpy(addr.sun_path, getenv("SWAYSOCK"), sizeof(addr.sun_path) - 1);
	connect(this->event_sock, (struct sockaddr*) &addr, sizeof(addr));
	connect(this->msg_sock, (struct sockaddr*) &addr, sizeof(addr));
	pthread_t thread;
	pthread_create(&thread, NULL, poll, this);
	return this;
}
