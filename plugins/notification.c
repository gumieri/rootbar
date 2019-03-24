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
#include <stdint.h>
#include <unistd.h>
#include <gio/gio.h>

static const char* arg_names[] = {"exec"};

struct notification {
	const char* exec;
	char* summary, *body;
	uint32_t id;
};

static void dbus_method_call(GDBusConnection* connection, const gchar* sender, const gchar* object_path, const gchar* interface_name, const gchar* method_name, GVariant* parameters, GDBusMethodInvocation* invocation, gpointer data) {
	(void) sender;
	struct notification* this = data;
	if(strcmp(method_name, "GetServerInformation") == 0) {
		GVariant* ret = g_variant_new("(ssss)", "rootbar", "Scoopta", "0.0.1", "1.2");
		g_dbus_method_invocation_return_value(invocation, ret);
	} else if(strcmp(method_name, "Notify") == 0) {
		const gchar* app_name, *app_icon, *summary, *body;
		uint32_t replaces_id;
		GVariantIter* actions;
		GVariant* hints;
		int32_t expire_timeout;
		g_variant_get(parameters, "(&su&s&s&sasa{sv}i)", &app_name, &replaces_id, &app_icon, &summary, &body, &actions, &hints, &expire_timeout);
		g_variant_iter_free(actions);
		if(this->summary != NULL) {
			free(this->summary);
		}
		if(this->body != NULL) {
			free(this->body);
		}
		this->summary = strdup(summary);
		this->body = strdup(body);
		if(this->exec != NULL && access(this->exec, X_OK) == 0) {
			if(fork() == 0) {
				execlp(this->exec, this->summary, this->body, NULL);
			}
		}
		GVariant* ret = g_variant_new("(u)", ++this->id);
		g_dbus_method_invocation_return_value(invocation, ret);
	} else if(strcmp(method_name, "GetCapabilities") == 0) {
		GVariantBuilder* builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
		g_variant_builder_add(builder, "s", "body");
		GVariant* ret = g_variant_new("as", builder);
		g_variant_builder_unref(builder);
		g_dbus_method_invocation_return_value(invocation, ret);
	} else if(strcmp(method_name, "CloseNotification") == 0) {
		uint32_t id;
		g_variant_get(parameters, "(u)", &id);
		GVariant* param = g_variant_new("(uu)", id, 3);
		g_dbus_connection_emit_signal(connection, NULL, object_path, interface_name, "NotificationClosed", param, NULL);
	}
}

static void name_acquired(GDBusConnection* connection, const gchar* name, gpointer data) {
	(void) name;
	struct notification* this = data;

	//GetCapabilities return
	GDBusArgInfo* get_capabilities_ret = malloc(sizeof(GDBusArgInfo));
	get_capabilities_ret->ref_count = -1;
	get_capabilities_ret->name = NULL;
	get_capabilities_ret->signature = "as";
	get_capabilities_ret->annotations = NULL;

	//GetCapabilities args
	GDBusArgInfo** get_capabilities_args = calloc(2, sizeof(GDBusArgInfo*));
	get_capabilities_args[0] = get_capabilities_ret;
	get_capabilities_args[1] = NULL;

	//GetCapabilities method
	GDBusMethodInfo* get_capabilities = malloc(sizeof(GDBusMethodInfo));
	get_capabilities->ref_count = -1;
	get_capabilities->name = "GetCapabilities";
	get_capabilities->in_args = NULL;
	get_capabilities->out_args = get_capabilities_args;
	get_capabilities->annotations = NULL;

	//Notify return
	GDBusArgInfo* notify_ret = malloc(sizeof(GDBusArgInfo));
	notify_ret->ref_count = -1;
	notify_ret->name = NULL;
	notify_ret->signature = "u";
	notify_ret->annotations = NULL;

	//Notify out args
	GDBusArgInfo** notify_out_args = calloc(2, sizeof(GDBusArgInfo*));
	notify_out_args[0] = notify_ret;
	notify_out_args[1] = NULL;

	//Notify method app_name argument
	GDBusArgInfo* notify_app_name = malloc(sizeof(GDBusArgInfo));
	notify_app_name->ref_count = -1;
	notify_app_name->name = "app_name";
	notify_app_name->signature = "s";
	notify_app_name->annotations = NULL;

	//Notify method replaces_id argument
	GDBusArgInfo* notify_replaces_id = malloc(sizeof(GDBusArgInfo));
	notify_replaces_id->ref_count = -1;
	notify_replaces_id->name = "replaces_id";
	notify_replaces_id->signature = "u";
	notify_replaces_id->annotations = NULL;

	//Notify method app_icon argument
	GDBusArgInfo* notify_app_icon = malloc(sizeof(GDBusArgInfo));
	notify_app_icon->ref_count = -1;
	notify_app_icon->name = "app_icon";
	notify_app_icon->signature = "s";
	notify_app_icon->annotations = NULL;

	//Notify method summary argument
	GDBusArgInfo* notify_summary = malloc(sizeof(GDBusArgInfo));
	notify_summary->ref_count = -1;
	notify_summary->name = "summary";
	notify_summary->signature = "s";
	notify_summary->annotations = NULL;

	//Notify method body argument
	GDBusArgInfo* notify_body = malloc(sizeof(GDBusArgInfo));
	notify_body->ref_count = -1;
	notify_body->name = "body";
	notify_body->signature = "s";
	notify_body->annotations = NULL;

	//Notify method actions argument
	GDBusArgInfo* notify_actions = malloc(sizeof(GDBusArgInfo));
	notify_actions->ref_count = -1;
	notify_actions->name = "actions";
	notify_actions->signature = "as";
	notify_actions->annotations = NULL;

	//Notify method hints argument
	GDBusArgInfo* notify_hints = malloc(sizeof(GDBusArgInfo));
	notify_hints->ref_count = -1;
	notify_hints->name = "hints";
	notify_hints->signature = "a{sv}";
	notify_hints->annotations = NULL;

	//Notify method expire_timeout argument
	GDBusArgInfo* notify_expire_timeout = malloc(sizeof(GDBusArgInfo));
	notify_expire_timeout->ref_count = -1;
	notify_expire_timeout->name = "expire_timeout";
	notify_expire_timeout->signature = "i";
	notify_expire_timeout->annotations = NULL;

	//Notify method args
	GDBusArgInfo** notify_args = calloc(9, sizeof(GDBusArgInfo*));
	notify_args[0] = notify_app_name;
	notify_args[1] = notify_replaces_id;
	notify_args[2] = notify_app_icon;
	notify_args[3] = notify_summary;
	notify_args[4] = notify_body;
	notify_args[5] = notify_actions;
	notify_args[6] = notify_hints;
	notify_args[7] = notify_expire_timeout;
	notify_args[8] = NULL;

	//Notify method
	GDBusMethodInfo* notify = malloc(sizeof(GDBusMethodInfo));
	notify->ref_count = -1;
	notify->name = "Notify";
	notify->in_args = notify_args;
	notify->out_args = notify_out_args;
	notify->annotations = NULL;

	//CloseNotification method notification_id argument
	GDBusArgInfo* close_notification_id = malloc(sizeof(GDBusArgInfo));
	close_notification_id->ref_count = -1;
	close_notification_id->name = "id";
	close_notification_id->signature = "u";
	close_notification_id->annotations = NULL;

	//CloseNotification method args
	GDBusArgInfo** close_notification_args = calloc(2, sizeof(GDBusArgInfo*));
	close_notification_args[0] = close_notification_id;
	close_notification_args[1] = NULL;

	//CloseNotification method
	GDBusMethodInfo* close_notification = malloc(sizeof(GDBusMethodInfo));
	close_notification->ref_count = -1;
	close_notification->name = "CloseNotification";
	close_notification->in_args = close_notification_args;
	close_notification->out_args = NULL;
	close_notification->annotations = NULL;

	//GetServerInformation name argument
	GDBusArgInfo* get_server_information_name = malloc(sizeof(GDBusArgInfo));
	get_server_information_name->ref_count = -1;
	get_server_information_name->name = "name";
	get_server_information_name->signature = "s";
	get_server_information_name->annotations = NULL;

	//GetServerInformation vendor argument
	GDBusArgInfo* get_server_information_vendor = malloc(sizeof(GDBusArgInfo));
	get_server_information_vendor->ref_count = -1;
	get_server_information_vendor->name = "vendor";
	get_server_information_vendor->signature = "s";
	get_server_information_vendor->annotations = NULL;

	//GetServerInformation version argument
	GDBusArgInfo* get_server_information_version = malloc(sizeof(GDBusArgInfo));
	get_server_information_version->ref_count = -1;
	get_server_information_version->name = "version";
	get_server_information_version->signature = "s";
	get_server_information_version->annotations = NULL;

	//GetServerInformation spec_version argument
	GDBusArgInfo* get_server_information_spec_version = malloc(sizeof(GDBusArgInfo));
	get_server_information_spec_version->ref_count = -1;
	get_server_information_spec_version->name = "spec_version";
	get_server_information_spec_version->signature = "s";
	get_server_information_spec_version->annotations = NULL;

	//GetServerInformation args
	GDBusArgInfo** get_server_information_args = calloc(5, sizeof(GDBusArgInfo*));
	get_server_information_args[0] = get_server_information_name;
	get_server_information_args[1] = get_server_information_vendor;
	get_server_information_args[2] = get_server_information_version;
	get_server_information_args[3] = get_server_information_spec_version;
	get_server_information_args[4] = NULL;

	//GetServerInformation method
	GDBusMethodInfo* get_server_information = malloc(sizeof(GDBusMethodInfo));
	get_server_information->ref_count = -1;
	get_server_information->name = "GetServerInformation";
	get_server_information->in_args = NULL;
	get_server_information->out_args = get_server_information_args;
	get_server_information->annotations = NULL;

	//DBus methods
	GDBusMethodInfo** methods = calloc(5, sizeof(GDBusMethodInfo*));
	methods[0] = get_capabilities;
	methods[1] = notify;
	methods[2] = close_notification;
	methods[3] = get_server_information;
	methods[4] = NULL;

	//NotificationClosed id argument
	GDBusArgInfo* notification_closed_id = malloc(sizeof(GDBusArgInfo));
	notification_closed_id->ref_count = -1;
	notification_closed_id->name = "id";
	notification_closed_id->signature = "u";
	notification_closed_id->annotations = NULL;

	//NotificationClosed reason argument
	GDBusArgInfo* notification_closed_reason = malloc(sizeof(GDBusArgInfo));
	notification_closed_reason->ref_count = -1;
	notification_closed_reason->name = "reason";
	notification_closed_reason->signature = "u";
	notification_closed_reason->annotations = NULL;

	//NotificationClosed args
	GDBusArgInfo** notification_closed_args = calloc(3, sizeof(GDBusArgInfo*));
	notification_closed_args[0] = notification_closed_id;
	notification_closed_args[1] = notification_closed_reason;
	notification_closed_args[2] = NULL;

	//NotificationClosed signal
	GDBusSignalInfo* notification_closed = malloc(sizeof(GDBusSignalInfo));
	notification_closed->ref_count = -1;
	notification_closed->name = "NotificationClosed";
	notification_closed->args = notification_closed_args;
	notification_closed->annotations = NULL;

	//ActionInvoked id argument
	GDBusArgInfo* action_invoked_id = malloc(sizeof(GDBusArgInfo));
	action_invoked_id->ref_count = -1;
	action_invoked_id->name = "id";
	action_invoked_id->signature = "u";
	action_invoked_id->annotations = NULL;

	//ActionInvoked action_key argument
	GDBusArgInfo* action_invoked_action_key = malloc(sizeof(GDBusArgInfo));
	action_invoked_action_key->ref_count = -1;
	action_invoked_action_key->name = "action_key";
	action_invoked_action_key->signature = "s";
	action_invoked_action_key->annotations = NULL;

	//ActionInvoked args
	GDBusArgInfo** action_invoked_args = calloc(3, sizeof(GDBusArgInfo*));
	action_invoked_args[0] = action_invoked_id;
	action_invoked_args[1] = action_invoked_action_key;
	action_invoked_args[2] = NULL;

	//ActionInvoked signal
	GDBusSignalInfo* action_invoked = malloc(sizeof(GDBusSignalInfo));
	notification_closed->ref_count = -1;
	notification_closed->name = "ActionInvoked";
	notification_closed->args = action_invoked_args;
	notification_closed->annotations = NULL;

	//DBus signals
	GDBusSignalInfo** signals = calloc(3, sizeof(GDBusSignalInfo*));
	signals[0] = notification_closed;
	signals[1] = action_invoked;
	signals[2] = NULL;

	GDBusInterfaceInfo* interface = malloc(sizeof(GDBusInterfaceInfo));
	interface->ref_count = -1;
	interface->name = "org.freedesktop.Notifications";
	interface->methods = methods;
	interface->signals = signals;
	interface->signals = NULL;
	interface->properties = NULL;
	interface->annotations = NULL;

	GDBusInterfaceVTable vtable = {
		.method_call = dbus_method_call,
		.get_property = NULL,
		.set_property = NULL
	};

	g_dbus_connection_register_object(connection, "/org/freedesktop/Notifications", interface, &vtable, this, NULL, NULL);
}

static void name_lost(GDBusConnection* connection, const gchar* name, gpointer data) {
	(void) connection;
	(void) data;
	fprintf(stderr, "Lost connection for %s\n", name);
}

void* notification_init(struct map* props) {
	struct notification* this = calloc(1, sizeof(struct notification));
	this->exec = map_get(props, "exec");
	g_bus_own_name(G_BUS_TYPE_SESSION, "org.freedesktop.Notifications", G_BUS_NAME_OWNER_FLAGS_DO_NOT_QUEUE, NULL, name_acquired, name_lost, this, NULL);
	return this;
}

const char** notification_get_arg_names() {
	return arg_names;
}

size_t notification_get_arg_count() {
	return sizeof(arg_names) / sizeof(char*);
}

void notification_get_info(void* data, const char* format, char* out, size_t size) {
	struct notification* this = data;
	snprintf(out, size, format, this->summary, this->body);
}