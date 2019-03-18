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

#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <string.h>

struct map* map_init();

void map_free();

void map_put(struct map* map, char* key, char* value);

char* map_get(struct map* map, char* key);

size_t map_size(struct map* map);

#endif
