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

#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <map.h>
#include <stdint.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include <sway_ipc.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

void workspace_init(const char* output_name, GtkBox* box, bool show_all, const char* plugin_name, uint32_t padding, const char* bar_name);

#endif
