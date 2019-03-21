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

void memory_get_info(void* data, const char* format, char* out, size_t size) {
	(void) data;
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
	free(line);
	uint64_t used = mem_total - mem_avail;
	float mb = used / 1024.f;
	gb = mb / 1024.f;
	percentage = ((float) used / mem_total) * 100.f;
	snprintf(out, size, format, gb, "GB", percentage, "%");
}
