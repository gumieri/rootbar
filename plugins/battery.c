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
#include <gio/gio.h>

static GDBusProxy* proxy;
static GError* err = NULL;

void* battery_init(struct map* props) {
	(void) props;
	proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, NULL, "org.freedesktop.UPower", "/org/freedesktop/UPower/devices/battery_BAT0", "org.freedesktop.DBus.Properties", NULL, NULL);
	return NULL;
}

void battery_get_info(void* data, const char* format, char* out, size_t size) {
	(void) data;
	GVariant* percentage = g_dbus_proxy_call_sync(proxy, "Get", g_variant_new("(ss)", "org.freedesktop.UPower.Device", "Percentage"), G_DBUS_CALL_FLAGS_NONE, 2000, NULL, &err);
	GVariant* ret;
	double percent;
	g_variant_get(percentage, "(v)", &ret);
	g_variant_get(ret, "d", &percent);
	g_variant_unref(percentage);
	snprintf(out, size, format, percent, "%");
}
