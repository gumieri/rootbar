/*
 *  Copyright (C) 2019 Scoopta
 *  This file is part of BTBar
 *  BTBar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BTBar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BTBar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>

void config_load(struct map* map, const char* config) {
	FILE* file = fopen(config, "r");
	char* line = NULL;
	size_t size = 0;
	while(getline(&line, &size, file) != -1) {
		char* equals = strchr(line, '=');
		if(equals == NULL) {
			continue;
		}
		char* hash = strchr(line, '#');
		if(hash != NULL) {
			*hash = 0;
		}
		*equals = 0;
		char* value = equals + 1;
		size_t len = strlen(value);
		*(value + len - 1) = 0;
		map_put(map, line, value);
	}
	free(line);
	fclose(file);
}

char* config_get(struct map* config, const char* prefix, const char* suffix, char* def_opt) {
	size_t pref_len = strlen(prefix);
	size_t suff_len = strlen(suffix);
	char* key = malloc(pref_len + suff_len + 1);
	strcpy(key, prefix);
	strcat(key, suffix);
	char* opt = map_get(config, key);
	free(key);
	if(opt == NULL) {
		opt = def_opt;
	}
	return opt;
}
