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
#include <time.h>
#include <utils.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

static const char* arg_names[] = {"interval", "core"};

struct cpu {
	uint32_t percentage;
	time_t interval;
	const char* core;
};

static void sample(struct cpu* this, uint64_t* total, uint64_t* idle) {
	FILE* stat = fopen("/proc/stat", "r");
	char* line = NULL;
	size_t size = 0;
	char* cmp;
	if(this->core == NULL || strcmp(this->core, "total") == 0) {
		cmp = utils_concat("cpu", " ");
	} else {
		cmp = utils_concat("cpu", this->core);
	}
	while(getline(&line, &size, stat) != -1) {
		if(strncmp(line, cmp, 4) == 0) {
			char* space = strchr(line, ' ');
			size_t space_count = 0;
			while(space != NULL) {
				++space_count;
				*space = 0;
				space = strchr(space + 1, ' ');
			}
			char* time = line + strlen(line);
			if(strcmp(cmp, "cpu ") == 0) {
				space_count -= 2;
				time += 2;
			} else {
				--space_count;
				++time;
			}
			*total = 0;
			for(size_t count = 0; count < space_count; ++count) {
				if(count == 3) {
					*idle = strtol(time, NULL, 10);
				}
				*total += strtol(time, NULL, 10);
				time += strlen(time) + 1;
			}
		}
	}
	free(cmp);
	fclose(stat);
	free(line);
}

static void* poll(void* data) {
	struct cpu* this = data;
	while(true) {
		uint64_t total1, idle1, total2, idle2;
		sample(this, &total1, &idle1);
		utils_sleep_millis(this->interval);
		sample(this, &total2, &idle2);
		uint64_t total = total2 - total1;
		uint64_t idle = idle2 - idle1;
		float time_idle = (float) idle / total;
		float time_working = 1.f - time_idle;
		this->percentage = time_working * 100.f;
	}
	return NULL;
}

void* cpu_init(struct map* props) {
	struct cpu* this = calloc(1, sizeof(struct cpu));
	char* interval = map_get(props, "interval");
	this->core = map_get(props, "core");
	if(interval == NULL) {
		interval = "1000";
	}
	this->interval = strtol(interval, NULL, 10);
	pthread_t thread;
	pthread_create(&thread, NULL, poll, this);
	return this;
}

void cpu_get_info(void* data, const char* format, char* out, size_t size) {
	struct cpu* this = data;
	snprintf(out, size, format, this->percentage, "%");
}

const char** cpu_get_arg_names() {
	return arg_names;
}

size_t cpu_get_arg_count() {
	return sizeof(arg_names) / sizeof(char*);
}
