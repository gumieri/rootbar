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
#include <stdio.h>
#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>

static const char* arg_names[] = {"sink", "iconify"};

struct volume {
	uint64_t sink;
	float volume;
	bool mute;
	bool iconify;
};

void info(pa_context* ctx, const pa_sink_info* info, int32_t eol, void* data) {
	(void) ctx;
	(void) eol;
	if(info != NULL) {
		struct volume* this = data;
		this->volume = ((float) pa_cvolume_avg(&info->volume) / PA_VOLUME_NORM) * 100.f;
		this->mute = info->mute;
	}
}

void subscribe(pa_context* ctx, pa_subscription_event_type_t event, uint32_t index, void* data) {
	struct volume* this = data;
	if((event & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == PA_SUBSCRIPTION_EVENT_SINK && this->sink == index) {
		pa_context_get_sink_info_by_index(ctx, this->sink, info, this);
	}
}

void ready(pa_context* ctx, void* data) {
	if(pa_context_get_state(ctx) == PA_CONTEXT_READY) {
		pa_context_set_subscribe_callback(ctx, subscribe, data);
		pa_context_subscribe(ctx, PA_SUBSCRIPTION_MASK_SINK, NULL, NULL);
	}
}

void* volume_init(struct map* props) {
	struct volume* this = calloc(1, sizeof(struct volume));
	char* sink = map_get(props, "sink");
	if(sink == NULL) {
		fprintf(stderr, "No sink specified\n");
		sink = "0";
	}
	this->sink = strtol(sink, NULL, 10);
	char* iconify = map_get(props, "iconify");
	this->iconify = iconify != NULL && strcmp(iconify, "none") != 0;
	pa_glib_mainloop* loop = pa_glib_mainloop_new(NULL);
	pa_mainloop_api* api = pa_glib_mainloop_get_api(loop);
	pa_context* ctx = pa_context_new(api, NULL);
	pa_context_connect(ctx, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL);
	pa_context_set_state_callback(ctx, ready, this);
	return this;
}

const char** volume_get_arg_names() {
	return arg_names;
}

size_t volume_get_arg_count() {
	return sizeof(arg_names) / sizeof(char*);
}

void volume_get_info(void* data, const char* format, char* out, size_t size) {
	struct volume* this = data;
	float volume;
	if(this->iconify && this->mute) {
		volume = -this->volume;
	} else {
		volume = this->volume;
	}
	snprintf(out, size, format, volume, this->mute ? "true" : "false");
}
