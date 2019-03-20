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

#include <workspace.h>

struct tmp {
	struct workspace* this;
	GtkLabel* label;
	const char* status;
	char* name;
};

struct workspace {
	const char* output_name, *plugin_name, *bar_name;
	GtkBox* box;
	bool show_all;
	struct sway_ipc* ipc;
	struct map* labels;
	const char* inactive, *urgent, *focused, *visible;
};

static void for_each(GtkWidget* widget, gpointer data) {
	struct workspace* this = data;
	gtk_container_remove(GTK_CONTAINER(this->box), widget);
	gtk_widget_set_name(widget, this->inactive);
}

static gboolean idle_remove(gpointer data) {
	struct workspace* this = data;
	gtk_container_foreach(GTK_CONTAINER(this->box), for_each, this);
	return FALSE;
}

static gboolean idle_add(gpointer data) {
	struct tmp* tmp = data;
	gtk_widget_set_name(GTK_WIDGET(tmp->label), tmp->status);
	gtk_label_set_text(tmp->label, tmp->name);
	free(tmp->name);
	gtk_container_add(GTK_CONTAINER(tmp->this->box), GTK_WIDGET(tmp->label));
	gtk_widget_show_all(GTK_WIDGET(tmp->this->box));
	free(tmp);
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
		const char* num = json_object_get_string(json_object_object_get(obj, "num"));
		const char* name = json_object_get_string(json_object_object_get(obj, "name"));
		const char* output = json_object_get_string(json_object_object_get(obj, "output"));
		bool urgent = json_object_get_boolean(json_object_object_get(obj, "urgent"));
		bool focused = json_object_get_boolean(json_object_object_get(obj, "focused"));
		bool visible = json_object_get_boolean(json_object_object_get(obj, "visible"));
		if(this->show_all || strcmp(output, this->output_name) == 0) {
			GtkLabel* label = map_get(this->labels, num);
			if(label == NULL) {
				label = GTK_LABEL(gtk_label_new(name));
				g_object_ref(label);
				GtkStyleContext* context = gtk_widget_get_style_context(GTK_WIDGET(label));
				gtk_style_context_add_class(context, this->plugin_name);
				gtk_style_context_add_class(context, this->bar_name);
				gtk_widget_set_name(GTK_WIDGET(label), this->inactive);
				map_put_void(this->labels, num, label);
			}
			struct tmp* tmp = malloc(sizeof(struct tmp));
			tmp->this = this;
			tmp->label = label;
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

void workspace_init(const char* output_name, GtkBox* box, bool show_all, const char* plugin_name, uint32_t padding, const char* bar_name) {
	struct workspace* this = calloc(1, sizeof(struct workspace));
	this->output_name = output_name;
	this->box = box;
	this->show_all = show_all;
	this->plugin_name = plugin_name;
	this->bar_name = bar_name;
	this->labels = map_init_void();
	this->inactive = concat(plugin_name, "-inactive");
	this->urgent = concat(plugin_name, "-urgent");
	this->visible = concat(plugin_name, "-visible");
	this->focused = concat(plugin_name, "-focused");
	gtk_widget_set_name(GTK_WIDGET(box), plugin_name);
	gtk_box_set_spacing(box, padding);
	this->ipc = sway_ipc_init();
	ask_workspaces(this, NULL);
	sway_ipc_subscribe(this->ipc, SWAY_IPC_EVENT_WORKSPACE, ask_workspaces, this);
}
