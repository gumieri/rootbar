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
#include <map.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <config.h>
#include <signal.h>
#include <gtk/gtk.h>
#include <wayland-client.h>

static char* CONFIG_LOCATION;
static char* COLORS_LOCATION;
static struct map* config;

static void print_usage(char** argv) {
	char* slash = strrchr(argv[0], '/');
	uint64_t offset;
	if(slash == NULL) {
		offset = 0;
	} else {
		offset = (slash - argv[0]) + 1;
	}
	printf("%s [options] <bar name|all>\n", argv[0] + offset);
	printf("Options:\n");
	printf("--help\t-h\tDisplays this help message\n");
	printf("--conf\t-c\tSelects a config file to use\n");
	printf("--style\t-s\tSelects a stylesheet to use\n");
	printf("--color\t-C\tSelects a colors file to use\n");
	exit(0);
}

static struct map* parse_args(int argc, char** argv) {
	struct map* ret = map_init();
	struct map* abrev = map_init();
	map_put(abrev, "c", "conf");
	map_put(abrev, "s", "style");
	map_put(abrev, "C", "color");
	if(argc > 1) {
		for(uint8_t count = 1; count < argc; ++count) {
			if(strcmp(argv[count], "-h") == 0 || strcmp(argv[count], "--help") == 0) {
				print_usage(argv);
			} else {
				if(strncmp(argv[count], "--", 2) == 0) {
					map_put(ret, argv[count] + 2, argv[count + 1]);
					++count;
				} else if(strncmp(argv[count], "-", 1) == 0) {
					size_t len = strlen(argv[count]);
					for(uint8_t chr = 1; chr < len; ++chr) {
						char str[2] = {argv[count][chr], 0};
						map_put(ret, map_get(abrev, str), argv[++count]);
					}
				} else {
					map_put(ret, "_barname", argv[count]);
				}
			}
		}
	} else {
		print_usage(argv);
	}
	map_free(abrev);
	return ret;
}

void sig(int32_t signum) {
	(void) signum;
	exit(0);
}

int main(int argc, char** argv) {
	struct map* args = parse_args(argc, argv);
	char* bar_name = map_get(args, "_barname");
	const char* config_str = map_get(args, "conf");
	char* style_str = map_get(args, "style");
	char* color_str = map_get(args, "color");

	const char* home_dir = getenv("HOME");
	const char* config_dir = "/.config/rootbar";
	const char* color_f = "/.cache/wal/colors";

	CONFIG_LOCATION = malloc(strlen(home_dir) + strlen(config_dir) + 1);
	strcpy(CONFIG_LOCATION, home_dir);
	strcat(CONFIG_LOCATION, config_dir);
	config = map_init();

	COLORS_LOCATION = malloc(strlen(home_dir) + strlen(color_f) + 1);
	strcpy(COLORS_LOCATION, home_dir);
	strcat(COLORS_LOCATION, color_f);

	//Check if --conf was specified
	char* config_path;
	if(config_str == NULL) {
		const char* config_f = "/config";
		config_path = malloc(strlen(CONFIG_LOCATION) + strlen(config_f) + 1);
		strcpy(config_path, CONFIG_LOCATION);
		strcat(config_path, config_f);
	} else {
		config_path = malloc(strlen(config_str) + 1);
		strcpy(config_path, config_str);
	}
	if(access(config_path, R_OK) == 0) {
		config_load(config, config_path);
	} else {
		fprintf(stderr, "Config doesn't exist\n");
		exit(1);
	}
	free(config_path);

	//Check if --style was specified
	char* stylesheet;
	if(style_str == NULL) {
		style_str = map_get(config, "stylesheet");
		if(style_str == NULL) {
			const char* style_f = "/style.css";
			stylesheet = malloc(strlen(CONFIG_LOCATION) + strlen(style_f) + 1);
			strcpy(stylesheet, CONFIG_LOCATION);
			strcat(stylesheet, style_f);
		} else {
			stylesheet = malloc(strlen(CONFIG_LOCATION) + strlen(style_str) + 2);
			strcpy(stylesheet, CONFIG_LOCATION);
			strcat(stylesheet, "/");
			strcat(stylesheet, style_str);
		}
	} else {
		stylesheet = malloc(strlen(style_str) + 1);
		strcpy(stylesheet, style_str);
	}

	//Check if --color was specified
	char* color_path;
	if(color_str == NULL) {
		color_str = map_get(config, "colors");
		if(color_str == NULL) {
			color_path = malloc(strlen(COLORS_LOCATION) + 1);
			strcpy(color_path, COLORS_LOCATION);
		} else {
			color_path = malloc(strlen(CONFIG_LOCATION) + strlen(color_str) + 2);
			strcpy(color_path, CONFIG_LOCATION);
			strcat(color_path, "/");
			strcat(color_path, color_str);
		}
	} else {
		color_path = malloc(strlen(color_str) + 1);
		strcpy(color_path, color_str);
	}
	gtk_init(&argc, &argv);
	if(access(stylesheet, R_OK) == 0) {
		FILE* file = fopen(stylesheet, "r");
		fseek(file, 0, SEEK_END);
		ssize_t size = ftell(file);
		fseek(file, 0, SEEK_SET);
		char* data = malloc(size + 1);
		fread(data, 1, size, file);
		fclose(file);
		data[size] = 0;
		struct wl_list lines;
		struct node {
			char* line;
			struct wl_list link;
		};
		wl_list_init(&lines);
		if(access(color_path, R_OK) == 0) {
			file = fopen(color_path, "r");
			char* line = NULL;
			size_t line_size = 0;
			ssize_t line_l = 0;
			while((line_l = getline(&line, &line_size, file)) != -1) {
				struct node* entry = malloc(sizeof(struct node));
				line[line_l - 1] = 0;
				entry->line = malloc(line_l + 1);
				strcpy(entry->line, line);
				wl_list_insert(&lines, &entry->link);
			}
			fclose(file);
			free(line);
		}
		ssize_t count = wl_list_length(&lines) - 1;
		if(count > 99) {
			fprintf(stderr, "Woah there that's a lot of colors. Try having no more than 99, thanks\n");
			exit(1);
		}
		struct node* node;
		wl_list_for_each(node, &lines, link) {
			//Do --rootbar-color replace
			const char* color = node->line;
			const char* rootbar_color = "--rootbar-color";
			char count_str[3];
			snprintf(count_str, 3, "%lu", count--);
			char* needle = malloc(strlen(rootbar_color) + strlen(count_str) + 1);
			strcpy(needle, rootbar_color);
			strcat(needle, count_str);
			size_t color_len = strlen(color);
			size_t needle_len = strlen(needle);
			if(color_len > needle_len) {
				free(needle);
				fprintf(stderr, "What color format is this, try #FFFFFF\n");
				continue;
			}
			char* replace = strstr(data, needle);
			while(replace != NULL) {
				memcpy(replace, color, color_len);
				memset(replace + color_len, ' ', needle_len - color_len);
				replace = strstr(data, needle);
			}
			free(needle);


			//Do --rootbar-rgb-color replace
			if(color_len < 7) {
				fprintf(stderr, "What color format is this, try #FFFFFF\n");
				continue;
			}
			const char* rootbar_rgb_color = "--rootbar-rgb-color";
			needle = malloc(strlen(rootbar_rgb_color) + strlen(count_str) + 1);
			strcpy(needle, rootbar_rgb_color);
			strcat(needle, count_str);
			needle_len = strlen(needle);
			replace = strstr(data, needle);
			while(replace != NULL) {
				char r[3];
				char g[3];
				char b[3];
				memcpy(r, color + 1, 2);
				memcpy(g, color + 3, 2);
				memcpy(b, color + 5, 2);
				r[2] = 0;
				g[2] = 0;
				b[2] = 0;
				char rgb[14];
				snprintf(rgb, 14, "%ld, %ld, %ld", strtol(r, NULL, 16), strtol(g, NULL, 16), strtol(b, NULL, 16));
				size_t rgb_len = strlen(rgb);
				memcpy(replace, rgb, rgb_len);
				memset(replace + rgb_len, ' ', needle_len - rgb_len);
				replace = strstr(data, needle);
			}
			free(needle);
		}
		GtkCssProvider* css = gtk_css_provider_new();
		gtk_css_provider_load_from_data(css, data, strlen(data), NULL);
		gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	}
	if(strcmp(bar_name, "all") == 0) {
		char* bars = map_get(config, "bars");
		if(bars == NULL) {
			fprintf(stderr, "all specified but no bars in config\n");
			exit(1);
		}
		char* comma = strchr(bars, ',');
		size_t comma_count = 1;
		while(comma != NULL) {
			++comma_count;
			*comma = 0;
			comma = strchr(comma + 1, ',');
		}
		char* bar = bars;
		for(size_t count = 0; count < comma_count; ++count) {
			bar_init(config, bar, config_get(config, bar, "-output", NULL), CONFIG_LOCATION);
			bar += strlen(bar) + 1;
		}
	} else {
		bar_init(config, bar_name, config_get(config, bar_name, "-output", NULL), CONFIG_LOCATION);
	}
	struct sigaction sigact;
	sigact.sa_handler = sig;
	sigaction(SIGUSR1, &sigact, NULL);
	gtk_main();
	return 0;
}
