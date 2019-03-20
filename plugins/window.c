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

struct window {
	char* title;
};

static void window(const char* json_data, void* data) {
	struct window* this = data;
	struct json_object* json = json_tokener_parse(json_data);
	struct json_object* container = json_object_object_get(json, "container");
	struct json_object* name = json_object_object_get(container, "name");
	if(!json_object_is_type(name, json_type_null)) {
		const char* title = json_object_get_string(name);
		if(this->title != NULL) {
			free(this->title);
		}
		this->title = strdup(title);
	}
	json_object_put(json);
}

void* window_init(struct map* props) {
	(void) props;
	struct window* this = calloc(1, sizeof(struct window));
	struct sway_ipc* ipc = sway_ipc_init();
	sway_ipc_subscribe(ipc, SWAY_IPC_EVENT_WINDOW, window, this);
	return this;
}

void window_get_info(void* data, const char* format, char* out, size_t size) {
	struct window* this = data;
	snprintf(out, size, format, this->title);
}

