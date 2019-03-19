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

#include <map.h>
#include <stdbool.h>
#include <sway_ipc.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

static const char* args[] = {"hide_default"};

struct bind_mode {
	char* mode;
	bool hide_default;
};

static void mode(const char* json_data, void* data) {
	struct bind_mode* this = data;
	struct json_object* json = json_tokener_parse(json_data);
	struct json_object* change = json_object_object_get(json, "change");
	const char* mode = json_object_get_string(change);
	if(this->mode != NULL) {
		free(this->mode);
	}
	if(this->hide_default && strcmp(mode, "default") == 0) {
		this->mode = "";
	} else {
		this->mode = strdup(mode);
	}
	json_object_put(json);
}

void* bind_mode_init(struct map* props) {
	struct bind_mode* this = calloc(1, sizeof(struct bind_mode));
	char* hide_default = map_get(props, "hide_default");
	this->hide_default = hide_default != NULL && strcmp(hide_default, "true") == 0;
	struct sway_ipc* ipc = sway_ipc_init();
	sway_ipc_subscribe(ipc, SWAY_IPC_EVENT_MODE, mode, this);
	return this;
}

const char** bind_mode_get_arg_names() {
	return args;
}

size_t bind_mode_get_arg_count() {
	return 1;
}

void bind_mode_get_info(void* data, const char* format, char* out, size_t size) {
	struct bind_mode* this = data;
	snprintf(out, size, format, this->mode);
}

