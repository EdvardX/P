#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

// Define buffer sizes
#define LINE_BUFFER_SIZE 256
#define KEY_BUFFER_SIZE 50
#define VALUE_BUFFER_SIZE 50

// Function loads the configuration from a file into the Config structure
void load_config(const char *filename, Config *config) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening configuration file.\n");
        exit(EXIT_FAILURE);
    }

    char line[LINE_BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        char key[KEY_BUFFER_SIZE], value[VALUE_BUFFER_SIZE];
        // Splits the line into key and value
        sscanf(line, "%49[^=]=%49[^\n]", key, value);

        // Mapping the key values to the appropriate fields in the Config structure
        if (strcmp(key, "frog_size") == 0) config->frog_size = atoi(value);
        else if (strcmp(key, "frog_shape") == 0) config->frog_shape = value[0];
        else if (strcmp(key, "car_size") == 0) config->car_size = atoi(value);
        else if (strcmp(key, "car_shape") == 0) config->car_shape = value[0];
        else if (strcmp(key, "friendly_car_shape") == 0) config->friendly_car_shape = value[0];
        else if (strcmp(key, "stork_shape") == 0) config->stork_shape = value[0];
        else if (strcmp(key, "screen_width") == 0) config->screen_width = atoi(value);
        else if (strcmp(key, "screen_height") == 0) config->screen_height = atoi(value);
        else if (strcmp(key, "max_cars") == 0) config->max_cars = atoi(value);
        else if (strcmp(key, "max_friendly_cars") == 0) config->max_friendly_cars = atoi(value);
        else if (strcmp(key, "max_coins") == 0) config->max_coins = atoi(value);
        else if (strcmp(key, "max_obstacles") == 0) config->max_obstacles = atoi(value);
        else if (strcmp(key, "quit_time") == 0) config->quit_time = atoi(value);
        else if (strcmp(key, "proximity_threshold") == 0) config->proximity_threshold = atoi(value);
        else if (strcmp(key, "max_speed_level_1") == 0) config->max_speed_level_1 = atoi(value);
        else if (strcmp(key, "max_speed_level_2") == 0) config->max_speed_level_2 = atoi(value);
        else if (strcmp(key, "max_speed_level_3") == 0) config->max_speed_level_3 = atoi(value);
        else if (strcmp(key, "car_color") == 0) config->car_color = (short)atoi(value);
        else if (strcmp(key, "friendly_car_color") == 0) config->friendly_car_color = (short)atoi(value);
        else if (strcmp(key, "frog_color") == 0) config->frog_color = (short)atoi(value);
        else if (strcmp(key, "coin_color") == 0) config->coin_color = (short)atoi(value);
        else if (strcmp(key, "goal_color") == 0) config->goal_color = (short)atoi(value);
        else if (strcmp(key, "stork_color") == 0) config->stork_color = (short)atoi(value);
        else if (strcmp(key, "road_color") == 0) config->road_color = (short)atoi(value);
    }

    fclose(file); // Closes the file after reading
}
