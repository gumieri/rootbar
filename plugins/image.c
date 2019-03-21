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

static const char* arg_names[] = {"path"};

struct image {
	const char* path;
};

void* image_init(struct map* props) {
	struct image* this = malloc(sizeof(struct image));
	this->path = map_get(props, "path");
	return this;
}

const char** image_get_arg_names() {
	return arg_names;
}

size_t image_get_arg_count() {
	return sizeof(arg_names) / sizeof(char*);
}

bool image_is_image() {
	return true;
}

void image_get_info(void* data, const char* format, char* out, size_t size) {
	struct image* this = data;
	snprintf(out, size, format, this->path);
}
