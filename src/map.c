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

#include <map.h>

struct map {
	char* key;
	void* value;
	size_t size;
	struct map* head, *left, *right;
};

struct map* map_init() {
	struct map* map = calloc(1, sizeof(struct map));
	map->head = map;
	return map;
}

void map_free(struct map* map) {
	if(map->left != NULL) {
		map_free(map->left);
	}
	if(map->right != NULL) {
		map_free(map->right);
	}
	if(map->key != NULL) {
		free(map->key);
	}
	if(map->value != NULL) {
		free(map->value);
	}
	free(map);
}

void map_put(struct map* map, const char* key, const void* value) {
	if(map->key == NULL) {
		map->key = malloc(strlen(key) + 1);
		strcpy(map->key, key);
		map->value = malloc(strlen(value) + 1);
		strcpy(map->value, value);
		++map->head->size;
	} else if(strcmp(key, map->key) < 0) {
		if(map->left == NULL) {
			map->left = map_init();
			map->left->head = map->head;
		}
		map_put(map->left, key, value);
	} else if(strcmp(key, map->key) > 0) {
		if(map->right == NULL) {
			map->right = map_init();
			map->right->head = map->head;
		}
		map_put(map->right, key, value);
	} else {
		free(map->value);
		map->value = malloc(strlen(value) + 1);
		strcpy(map->value, value);
	}
}

void* map_get(struct map* map, const char* key) {
	if(map->key == NULL) {
		return NULL;
	} else if(strcmp(key, map->key) < 0) {
		if(map->left == NULL) {
			return NULL;
		}
		return map_get(map->left, key);
	} else if(strcmp(key, map->key) > 0) {
		if(map->right == NULL) {
			return NULL;
		}
		return map_get(map->right, key);
	} else {
		return map->value;
	}
}

size_t map_size(struct map* map) {
	return map->size;
}
