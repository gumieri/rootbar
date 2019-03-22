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
#include <stdint.h>

static const char* arg_names[] = {"percent"};

struct memory {
	bool percent;
};

void* memory_init(struct map* props) {
	struct memory* this = malloc(sizeof(struct memory));
	const char* percent = map_get(props, "percent");
	this->percent = percent != NULL && strcmp(percent, "true") == 0;
	return this;
}

const char** memory_get_arg_names() {
	return arg_names;
}

size_t memory_get_arg_count() {
	return sizeof(arg_names) / sizeof(char*);
}

void memory_get_info(void* data, const char* format, char* out, size_t size) {
	struct memory* this = data;
	float gb, percentage;
	FILE* meminfo = fopen("/proc/meminfo", "r");
	char* line = NULL;
	size_t line_s = 0;
	uint64_t mem_total = 0, mem_avail = 0;
	while(getline(&line, &line_s, meminfo) != -1) {
		const char* total = "MemTotal:";
		const char* avail = "MemAvailable:";
		char* info = strchr(line, ' ');
		while(*info == ' ') {
			++info;
		}
		char* space = strchr(info, ' ');
		if(space != NULL) {
			*space = 0;
		}
		if(strncmp(line, total, strlen(total)) == 0) {
			mem_total = strtol(info, NULL, 10);
		} else if(strncmp(line, avail, strlen(avail)) == 0) {
			mem_avail = strtol(info, NULL, 10);
		}
	}
	fclose(meminfo);
	free(line);
	uint64_t used = mem_total - mem_avail;
	if(this->percent) {
		percentage = ((float) used / mem_total) * 100.f;
		snprintf(out, size, format, percentage, "%");
	} else {
		float mb = used / 1024.f;
		gb = mb / 1024.f;
		snprintf(out, size, format, gb, "GB");
	}
}
