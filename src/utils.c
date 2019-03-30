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

#include <utils.h>

time_t utils_get_time_millis() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

void utils_sleep_millis(time_t millis) {
	struct timespec time;
	time.tv_sec = millis / 1000;
	time.tv_nsec = (millis % 1000) * pow(1000, 2);
	nanosleep(&time, NULL);
}

char* utils_concat(const char* str1, const char* str2) {
	char* buffer = malloc(strlen(str1) + strlen(str2) + 1);
	strcpy(buffer, str1);
	strcat(buffer, str2);
	return buffer;
}

size_t utils_split(char* str, const char chr) {
	char* split = strchr(str, chr);
	size_t count = 1;
	for(; split != NULL; ++count) {
		*split = 0;
		split = strchr(split + 1, chr);
	}
	return count;
}
