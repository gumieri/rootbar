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
#include <unistd.h>

static const char* arg_names[] = {"exec"};

struct external {
	const char* exec;
};

void* external_init(struct map* props) {
	struct external* this = malloc(sizeof(struct external));
	this->exec = map_get(props, "exec");
	return this;
}

const char** external_get_arg_names() {
	return arg_names;
}

size_t external_get_arg_count() {
	return sizeof(arg_names) / sizeof(char*);
}

void external_get_info(void* data, const char* format, char* out, size_t size) {
	struct external* this = data;
	if(this->exec == NULL) {
		fprintf(stderr, "That's not a valid executable");
		return;
	}
	if(access(this->exec, X_OK) == 0) {
		FILE* proc = popen(this->exec, "r");
		char* line = NULL;
		size_t line_s = 0;
		ssize_t line_l = getline(&line, &line_s, proc);
		line[line_l - 1] = 0;
		pclose(proc);
		snprintf(out, size, format, line);
		free(line);
	} else {
		fprintf(stderr, "%s cannot be found\n", this->exec);
	}
}

