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

#include <bar.h>

struct output_node {
	char* name;
	struct wl_output* output;
	int32_t width, height;
	struct wl_list link;
};

struct plugin_node {
	void* plugin;
	GtkWidget* widget;
	char* format;
	uint64_t length;
	void (*get_info)(void* data, const char* format, char* out, size_t size);
	struct wl_list link;
};

struct bar {
	struct output_node* output;
	struct wl_list outputs;
	struct zxdg_output_manager_v1* output_manager;
	struct zwlr_layer_shell_v1* shell;
	uint64_t width, height;
	const char* location;
	GtkWidget* window;
	struct wl_list plugins;
	uint64_t interval;
};

static void nop() {}

static void add_interface(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version) {
	struct bar* this = data;
	if(strcmp(interface, wl_output_interface.name) == 0) {
		struct output_node* node = malloc(sizeof(struct output_node));
		node->output = wl_registry_bind(registry, name, &wl_output_interface, version);
		wl_list_insert(&this->outputs, &node->link);
	} else if(strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
		this->output_manager = wl_registry_bind(registry, name, &zxdg_output_manager_v1_interface, version);
	} else if(strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
		this->shell = wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface, version);
	}
}

static void get_name(void* data, struct zxdg_output_v1* output, const char* name) {
	(void) output;
	struct output_node* node = data;
	node->name = malloc(strlen(name) + 1);
	strcpy(node->name, name);
}

static void get_res(void* data, struct zxdg_output_v1* output, int32_t width, int32_t height) {
	(void) output;
	struct output_node* node = data;
	node->width = width;
	node->height = height;
}

static void config_surface(void* data, struct zwlr_layer_surface_v1* surface, uint32_t serial, uint32_t width, uint32_t height) {
	(void) width;
	(void) height;
	struct bar* this = data;
	zwlr_layer_surface_v1_ack_configure(surface, serial);
	zwlr_layer_surface_v1_set_exclusive_zone(surface, this->height);
	zwlr_layer_surface_v1_set_size(surface, this->width, this->height);
	enum zwlr_layer_surface_v1_anchor location = strcmp(this->location, "bottom") == 0 ? ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM : ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP;
	zwlr_layer_surface_v1_set_anchor(surface, ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT | location | ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);
	gtk_window_set_default_size(GTK_WINDOW(this->window), this->width, this->height);
	gtk_widget_show_all(this->window);
}

static gboolean idle_add(gpointer data) {
	struct bar* this = data;
	struct plugin_node* node;
	wl_list_for_each(node, &this->plugins, link) {
		char output[node->length + 1];
		node->get_info(node->plugin, node->format, output, node->length + 1);
		if(strlen(output) == node->length) {
			for(size_t count = 0; count < 3; ++count) {
				output[node->length - 1 - count] = '.';
			}
		}
		char* lf = strchr(output, '\n');
		if(lf != NULL) {
			*lf = 0;
		}
		gtk_label_set_text(GTK_LABEL(node->widget), output);
	}
	return FALSE;
}

static void* run(void* data) {
	struct bar* this = data;
	while(true) {
		g_idle_add(idle_add, this);
		utils_sleep_millis(this->interval);
	}
	return NULL;
}

static void* get_plugin_func(const char* prefix, const char* suffix) {
	void* fun_name = malloc(strlen(prefix) + strlen(suffix) + 1);
	strcpy(fun_name, prefix);
	strcat(fun_name, suffix);
	void* fun = dlsym(RTLD_DEFAULT, fun_name);
	free(fun_name);
	return fun;
}

void bar_init(struct map* config, const char* bar_name, char* output_name, const char* config_location) {
	if(output_name == NULL) {
		fprintf(stderr, "No output specified for %s\n", bar_name);
		return;
	}
	struct bar* this = calloc(1, sizeof(struct bar));
	wl_list_init(&this->outputs);
	wl_list_init(&this->plugins);
	this->location = config_get(config, bar_name, "-location", "top");
	this->interval = strtol(config_get(config, bar_name, "-interval", "1000"), NULL, 10);
	this->height = strtol(config_get(config, bar_name, "-height", "30"), NULL, 10);
	GdkDisplay* disp = gdk_display_get_default();
	struct wl_display* wl = gdk_wayland_display_get_wl_display(disp);
	struct wl_registry* registry = wl_display_get_registry(wl);
	struct wl_registry_listener reg_listener = {
		.global = add_interface,
		.global_remove = nop
	};
	wl_registry_add_listener(registry, &reg_listener, this);
	wl_display_roundtrip(wl);

	struct output_node* node;
	wl_list_for_each(node, &this->outputs, link) {
		struct zxdg_output_v1* xdg_output = zxdg_output_manager_v1_get_xdg_output(this->output_manager, node->output);
		struct zxdg_output_v1_listener xdg_listener = {
			.description = nop,
			.done = nop,
			.logical_position = nop,
			.logical_size = get_res,
			.name = get_name
		};
		zxdg_output_v1_add_listener(xdg_output, &xdg_listener, node);
	}
	wl_display_roundtrip(wl);

	wl_list_for_each(node, &this->outputs, link) {
		if(strcmp(output_name, node->name) == 0) {
			this->output = node;
		}
	}

	if(this->output == NULL) {
		fprintf(stderr, "Bad output name %s\n", output_name);
		return;
	}

	const char* width_str = config_get(config, bar_name, "-width", NULL);
	if(width_str == NULL) {
		this->width = this->output->width;
	} else {
		this->width = strtol(width_str, NULL, 10);
	}

	this->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_object_ref(this->window);
	gtk_widget_realize(this->window);
	GdkWindow* gdk_win = gtk_widget_get_window(this->window);
	gdk_wayland_window_set_use_custom_surface(gdk_win);
	gtk_widget_set_name(this->window, bar_name);
	gtk_style_context_add_class(gtk_widget_get_style_context(this->window), bar_name);

	struct wl_surface* wl_surface = gdk_wayland_window_get_wl_surface(gdk_win);
	struct zwlr_layer_surface_v1* surface = zwlr_layer_shell_v1_get_layer_surface(this->shell, wl_surface, this->output->output, ZWLR_LAYER_SHELL_V1_LAYER_TOP, "btbar");

	struct zwlr_layer_surface_v1_listener* surface_listener = malloc(sizeof(struct zwlr_layer_surface_v1_listener));
	surface_listener->configure = config_surface;
	surface_listener->closed = nop;
	zwlr_layer_surface_v1_add_listener(surface, surface_listener, this);
	wl_surface_commit(wl_surface);
	wl_display_roundtrip(wl);

	GtkContainer* root = GTK_CONTAINER(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
	gtk_box_set_homogeneous(GTK_BOX(root), TRUE);
	gtk_container_add(GTK_CONTAINER(this->window), GTK_WIDGET(root));

	GtkWidget* left = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(left, "left");
	GtkStyleContext* context = gtk_widget_get_style_context(left);
	gtk_style_context_add_class(context, "left");

	GtkWidget* center = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(center, "center");
	context = gtk_widget_get_style_context(center);
	gtk_style_context_add_class(context, "center");

	GtkWidget* center_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_center_widget(GTK_BOX(center), center_box);

	GtkWidget* right = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(right, "right");
	context = gtk_widget_get_style_context(right);
	gtk_style_context_add_class(context, "right");

	gtk_container_add(root, left);
	gtk_container_add(root, center);
	gtk_container_add(root, right);

	size_t comma_count = 1;
	char* plugin_names = config_get(config, bar_name, "-plugins", NULL);
	if(plugin_names == NULL) {
		return;
	}
	char* comma = strchr(plugin_names, ',');
	while(comma != NULL) {
		++comma_count;
		*comma = 0;
		comma = strchr(comma + 1, ',');
	}
	char* plugin_name = plugin_names;
	for(size_t count = 0; count < comma_count; ++count) {
		char* dso_name = config_get(config, plugin_name, "-dso", NULL);
		if(dso_name == NULL) {
			fprintf(stderr, "No DSO specified for %s\n", plugin_name);
			goto plugin_incr;
		}

		uint64_t padding = strtol(config_get(config, plugin_name, "-padding", "10"), NULL, 10);
		char* dso = strstr(dso_name, ".so");
		void* (*init)(struct map* props);
		void (*init_adv)(struct map* props, GtkBox* box);
		const char** (*get_arg_names)();
		size_t (*get_arg_count)();
		bool (*is_advanced)();
		void (*get_info)(void* data, const char* format, char* out, size_t size);
		if(dso == NULL) {
			char* init_str = "_init";
			char* get_arg_names_str = "_get_arg_names";
			char* get_arg_count_str = "_get_arg_count";
			char* is_advanced_str = "_is_advanced";
			char* get_info_str = "_get_info";
			init = get_plugin_func(dso_name, init_str);
			init_adv = get_plugin_func(dso_name, init_str);
			get_arg_names = get_plugin_func(dso_name, get_arg_names_str);
			get_arg_count = get_plugin_func(dso_name, get_arg_count_str);
			is_advanced = get_plugin_func(dso_name, is_advanced_str);
			get_info = get_plugin_func(dso_name, get_info_str);
		} else {
			char* plugins_dir = utils_concat(config_location, "/plugins/");
			char* full_name = utils_concat(plugins_dir, dso_name);
			void* plugin = dlopen(full_name, RTLD_LAZY);
			free(full_name);
			free(plugins_dir);
			init = dlsym(plugin, "init");
			init_adv = dlsym(plugin, "init");
			get_arg_names = dlsym(plugin, "get_arg_names");
			get_arg_count = dlsym(plugin, "get_arg_count");
			is_advanced = dlsym(plugin, "is_advanced");
			get_info = dlsym(plugin, "get_info");
		}

		bool is_adv = is_advanced != NULL && is_advanced();

		if(get_info == NULL && !is_adv) {
			fprintf(stderr, "%s has no get_info function\n", plugin_name);
			goto plugin_incr;
		}

		const char** arg_names = NULL;
		size_t arg_count = 0;
		if(get_arg_names != NULL && get_arg_count != NULL) {
			arg_names = get_arg_names();
			arg_count = get_arg_count();
		}

		struct map* props = map_init();
		for(size_t count = 0; count < arg_count; ++count) {
			const char* arg = arg_names[count];
			char* hyphen_name = malloc(strlen(arg) + 2);
			strcpy(hyphen_name, "-");
			strcat(hyphen_name, arg);
			map_put(props, arg, config_get(config, plugin_name, hyphen_name, NULL));
			free(hyphen_name);
		}

		GtkWidget* widget;

		if(is_adv) {
			widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
			map_put(props, "_output", output_name);
			map_put(props, "_plugin", plugin_name);
			init_adv(props, GTK_BOX(widget));
		} else {
			void* plugin = NULL;
			if(init != NULL) {
				plugin = init(props);
			}
			struct plugin_node* node = malloc(sizeof(struct plugin_node));
			widget = gtk_label_new(NULL);
			node->widget = widget;
			node->plugin = plugin;
			node->format = config_get(config, plugin_name, "-format", "%s");
			node->length = strtol(config_get(config, plugin_name, "-length", "15"), NULL, 10);
			node->get_info = get_info;
			wl_list_insert(&this->plugins, &node->link);
		}
		gtk_widget_set_name(widget, plugin_name);
		char* position = config_get(config, plugin_name, "-position", "right");
		if(strcmp(position, "left") == 0) {
			gtk_box_pack_start(GTK_BOX(left), widget, false, false, padding);
		} else if(strcmp(position, "center") == 0) {
			gtk_box_pack_start(GTK_BOX(center_box), widget, false, false, padding);
		} else if(strcmp(position, "right") == 0) {
			gtk_box_pack_end(GTK_BOX(right), widget, false, false, padding);
		}
		pthread_t thread;
		pthread_create(&thread, NULL, run, this);
		plugin_incr:
			plugin_name += strlen(plugin_name) + 1;
	}
}
