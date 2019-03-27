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
#include <unistd.h>

static const char* arg_names[] = {"text", "exec"};

struct action {
	const char* text, *exec;
};

void* action_init(struct map* props) {
	struct action* this = malloc(sizeof(struct action));
	this->text = map_get(props, "text");
	this->exec = map_get(props, "exec");
	return this;
}

const char** action_get_arg_names() {
	return arg_names;
}

size_t action_get_arg_count() {
	return sizeof(arg_names) / sizeof(char*);
}

void action_get_info(void* data, const char* format, char* out, size_t size) {
	struct action* this = data;
	snprintf(out, size, format, this->text);
}

void action_click(void* data) {
	struct action* this = data;
	if(this->exec != NULL) {
		if(access(this->exec, X_OK) == 0) {
			if(fork() == 0) {
				execlp(this->exec, this->exec, this->text, NULL);
			}
		} else {
			fprintf(stderr, "%s cannot be found\n", this->exec);
		}
	} else {
		fprintf(stderr, "That's not a valid executable\n");
	}
}
