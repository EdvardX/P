#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

// Define buffer sizes
#define LINE_BUFFER_SIZE 256
#define KEY_BUFFER_SIZE 50
#define VALUE_BUFFER_SIZE 50

// Map general configuration values to the Config structure
void map_general_config(Config *config, const char *key, const char *value) {
    if (strcmp(key, "frog_size") == 0) config->frog_size = atoi(value);
    else if (strcmp(key, "frog_shape") == 0) config->frog_shape = value[0];
    else if (strcmp(key, "car_size") == 0) config->car_size = atoi(value);
    else if (strcmp(key, "car_shape") == 0) config->car_shape = value[0];
    else if (strcmp(key, "friendly_car_shape") == 0) config->friendly_car_shape = value[0];
    else if (strcmp(key, "stork_shape") == 0) config->stork_shape = value[0];
    else if (strcmp(key, "screen_width") == 0) config->screen_width = atoi(value);
    else if (strcmp(key, "screen_height") == 0) config->screen_height = atoi(value);
}

// Map maximum values to the Config structure
void map_maximums_config(Config *config, const char *key, const char *value) {
    if (strcmp(key, "max_cars") == 0) config->max_cars = atoi(value);
    else if (strcmp(key, "max_friendly_cars") == 0) config->max_friendly_cars = atoi(value);
    else if (strcmp(key, "max_coins") == 0) config->max_coins = atoi(value);
    else if (strcmp(key, "max_obstacles") == 0) config->max_obstacles = atoi(value);
    else if (strcmp(key, "quit_time") == 0) config->quit_time = atoi(value);
    else if (strcmp(key, "proximity_threshold") == 0) config->proximity_threshold = atoi(value);
}

// Map speed values to the Config structure
void map_speed_config(Config *config, const char *key, const char *value) {
    if (strcmp(key, "max_speed_level_1") == 0) config->max_speed_level_1 = atoi(value);
    else if (strcmp(key, "max_speed_level_2") == 0) config->max_speed_level_2 = atoi(value);
    else if (strcmp(key, "max_speed_level_3") == 0) config->max_speed_level_3 = atoi(value);
}

// Map color values to the Config structure
void map_color_config(Config *config, const char *key, const char *value) {
    if (strcmp(key, "car_color") == 0) config->car_color = (short)atoi(value);
    else if (strcmp(key, "friendly_car_color") == 0) config->friendly_car_color = (short)atoi(value);
    else if (strcmp(key, "frog_color") == 0) config->frog_color = (short)atoi(value);
    else if (strcmp(key, "coin_color") == 0) config->coin_color = (short)atoi(value);
    else if (strcmp(key, "goal_color") == 0) config->goal_color = (short)atoi(value);
    else if (strcmp(key, "stork_color") == 0) config->stork_color = (short)atoi(value);
    else if (strcmp(key, "road_color") == 0) config->road_color = (short)atoi(value);
    else if (strcmp(key, "obstacles_color") == 0) config->obstacles_color = (short)atoi(value);
 else if (strcmp(key, "lane_color") == 0) config->lane_color = (short)atoi(value);
}


// Map speed and color values to the Config structure
void map_speed_color_config(Config *config, const char *key, const char *value) {
    map_speed_config(config, key, value);
    map_color_config(config, key, value);
}

// Map the configuration values to the Config structure by checking each category
void map_config(Config *config, const char *key, const char *value) {
    map_general_config(config, key, value);
    map_maximums_config(config, key, value);
    map_speed_color_config(config, key, value);
}

// Open the configuration file and handle errors if the file cannot be opened
FILE* open_config_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening configuration file.\n");
        exit(EXIT_FAILURE);
    }
    return file;
}

// Read a line from the file into the provided buffer
#define READ_LINE(file, line, size) fgets(line, (int)(size), file)

void read_line(FILE *file, char *line, size_t size) {
    if (READ_LINE(file, line, size) == NULL) {
        line[0] = '\0'; // Clear line in case of EOF or read error
    }
}

// Parse a line from the configuration file into key and value components
void parse_line(char *line, char *key, char *value) {
    sscanf(line, "%49[^=]=%49[^\n]", key, value);
}

// Load the configuration from the specified file into the Config structure
void load_config(const char *filename, Config *config) {
    FILE *file = open_config_file(filename);
    
    char line[LINE_BUFFER_SIZE];
    char key[KEY_BUFFER_SIZE], value[VALUE_BUFFER_SIZE];

    while (1) {
        read_line(file, line, sizeof(line));
        if (line[0] == '\0') break; // End of file or error

        parse_line(line, key, value);
        map_config(config, key, value);
    }

    fclose(file);
}
