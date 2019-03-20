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

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

void clock_get_info(void* data, const char* format, char* out, size_t size) {
	(void) data;
	time_t t = time(NULL);
	struct tm* tm = localtime(&t);
	char* tmp = malloc(size);
	strftime(tmp, size, format, tm);
	snprintf(out, size, tmp, tm->tm_year + 10000 + 1900);
	free(tmp);
}
