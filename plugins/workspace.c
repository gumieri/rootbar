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

#include <stdbool.h>
#include <gtk/gtk.h>
#include <sway_ipc.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

static const char* arg_names[] = {"show_all", "padding"};

struct tmp {
	struct workspace* this;
	GtkEventBox* box;
	const char* status;
	char* name;
};

struct click_info {
	struct workspace* this;
	const char* name;
};

struct workspace {
	const char* output_name, *plugin_name;
	GtkBox* box;
	bool show_all;
	struct sway_ipc* ipc, *click_ipc;
	struct map* labels;
	const char* inactive, *urgent, *focused, *visible;
};

static void for_each(GtkWidget* widget, gpointer data) {
	struct workspace* this = data;
	gtk_container_remove(GTK_CONTAINER(this->box), widget);
	GtkWidget* label = gtk_bin_get_child(GTK_BIN(widget));
	gtk_widget_set_name(label, this->inactive);
}

static gboolean idle_remove(gpointer data) {
	struct workspace* this = data;
	gtk_container_foreach(GTK_CONTAINER(this->box), for_each, this);
	return FALSE;
}

static gboolean idle_add(gpointer data) {
	struct tmp* tmp = data;
	GtkLabel* label = GTK_LABEL(gtk_bin_get_child(GTK_BIN(tmp->box)));
	gtk_widget_set_name(GTK_WIDGET(label), tmp->status);
	gtk_label_set_text(label, tmp->name);
	free(tmp->name);
	gtk_container_add(GTK_CONTAINER(tmp->this->box), GTK_WIDGET(tmp->box));
	gtk_widget_show_all(GTK_WIDGET(tmp->this->box));
	free(tmp);
	return FALSE;
}

static gboolean click(GtkWidget* widget, GdkEvent* event, gpointer data) {
	(void) widget;
	(void) event;
	struct click_info* info = data;
	struct workspace* this = info->this;
	char* cmd = utils_concat("workspace ", info->name);
	char* payload = sway_ipc_send_message(this->click_ipc, SWAY_IPC_MESSAGE_RUN_COMMAND, cmd, SWAY_IPC_REPLY_COMMAND);
	free(cmd);
	free(payload);
	return FALSE;
}

static void ask_workspaces(void* data, const char* ignored) {
	(void) ignored;
	struct workspace* this = data;
	char* payload = sway_ipc_send_message(this->ipc, SWAY_IPC_MESSAGE_GET_WORKSPACES, NULL, SWAY_IPC_REPLY_WORKSPACES);
	struct json_object* arr = json_tokener_parse(payload);
	free(payload);
	size_t arr_s = json_object_array_length(arr);
	g_idle_add(idle_remove, this);
	for(size_t count = 0; count < arr_s; ++count) {
		struct json_object* obj = json_object_array_get_idx(arr, count);
		const char* name = json_object_get_string(json_object_object_get(obj, "name"));
		const char* output = json_object_get_string(json_object_object_get(obj, "output"));
		bool urgent = json_object_get_boolean(json_object_object_get(obj, "urgent"));
		bool focused = json_object_get_boolean(json_object_object_get(obj, "focused"));
		bool visible = json_object_get_boolean(json_object_object_get(obj, "visible"));
		if(this->show_all || strcmp(output, this->output_name) == 0) {
			GtkEventBox* box = map_get(this->labels, name);
			if(box == NULL) {
				box = GTK_EVENT_BOX(gtk_event_box_new());
				g_object_ref(box);
				GtkWidget* label = gtk_label_new(name);
				GtkStyleContext* context = gtk_widget_get_style_context(GTK_WIDGET(label));
				gtk_style_context_add_class(context, this->plugin_name);
				gtk_widget_set_name(GTK_WIDGET(label), this->inactive);
				gtk_container_add(GTK_CONTAINER(box), label);
				gtk_widget_add_events(GTK_WIDGET(box), GDK_BUTTON_PRESS);
				struct click_info* info = malloc(sizeof(struct click_info));
				info->name = strdup(name);
				info->this = this;
				g_signal_connect(GTK_WIDGET(box), "button-press-event", G_CALLBACK(click), info);
				map_put_void(this->labels, name, box);
			}
			struct tmp* tmp = malloc(sizeof(struct tmp));
			tmp->this = this;
			tmp->box = box;
			tmp->name = strdup(name);
			if(urgent) {
				tmp->status = this->urgent;
			} else if(focused) {
				tmp->status = this->focused;
			} else if(visible) {
				tmp->status = this->visible;
			} else {
				tmp->status = this->inactive;
			}
			g_idle_add(idle_add, tmp);
		}
	}
	json_object_put(arr);
}

static char* concat(const char* plugin_name, const char* status) {
	char* buffer = malloc(strlen(plugin_name) + strlen(status) + 1);
	strcpy(buffer, plugin_name);
	strcat(buffer, status);
	return buffer;
}

void workspace_init(struct map* props, GtkBox* box) {
	struct workspace* this = calloc(1, sizeof(struct workspace));
	this->output_name = map_get(props, "_output");
	this->box = box;
	char* show_all_str = map_get(props, "show_all");
	this->show_all = show_all_str != NULL && strcmp(show_all_str, "true") == 0;
	this->plugin_name = map_get(props, "_plugin");
	this->labels = map_init_void();
	this->inactive = concat(this->plugin_name, "-inactive");
	this->urgent = concat(this->plugin_name, "-urgent");
	this->visible = concat(this->plugin_name, "-visible");
	this->focused = concat(this->plugin_name, "-focused");
	char* padding_str = map_get(props, "padding");
	uint64_t padding;
	if(padding_str == NULL) {
		padding = 20;
	} else {
		padding = strtol(padding_str, NULL, 10);
	}
	gtk_widget_set_name(GTK_WIDGET(box), this->plugin_name);
	gtk_box_set_spacing(box, padding);
	this->ipc = sway_ipc_init();
	this->click_ipc = sway_ipc_init();
	ask_workspaces(this, NULL);
	sway_ipc_subscribe(this->ipc, SWAY_IPC_EVENT_WORKSPACE, ask_workspaces, this);
}

bool workspace_is_advanced() {
	return true;
}

const char** workspace_get_arg_names() {
	return arg_names;
}

size_t workspace_get_arg_count() {
	return sizeof(arg_names) / sizeof(char*);
}
