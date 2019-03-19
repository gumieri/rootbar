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

#ifndef BAR_H
#define BAR_H

#define _GNU_SOURCE
#include <map.h>
#include <utils.h>
#include <dlfcn.h>
#include <stdio.h>
#include <config.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <gdk/gdkwayland.h>
#include <wayland-client.h>
#include <xdg-output-unstable-v1-client-protocol.h>
#include <wlr-layer-shell-unstable-v1-client-protocol.h>

void bar_init(struct map* config, const char* bar_name, const char* output_name);


#endif
