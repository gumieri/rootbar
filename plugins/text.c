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

static const char* args[] = {"text"};

struct text {
	char* text;
};

void* text_init(struct map* props) {
	struct text* this = malloc(sizeof(struct text));
	this->text = map_get(props, "text");
	return this;
}

const char** text_get_arg_names() {
	return args;
}

size_t text_get_arg_count() {
	return 1;
}

void text_get_info(void* data, const char* format, char* out, size_t size) {
	struct text* this = data;
	snprintf(out, size, format, this->text);
}
