#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h> // Required for EXIT_FAILURE constant

#define MAX_CARS 9
#define MAX_FRIENDLY_CARS 2
#define MAX_COINS 5
#define MAX_OBSTACLES 20

// Config structure stores all game configuration settings.
typedef struct Config {
    int frog_size;
    char frog_shape;
    int car_size;
    char car_shape;
    char friendly_car_shape;
    char stork_shape;
    int screen_width;
    int screen_height;
    int max_cars;
    int max_friendly_cars;
    int max_coins;
    int max_obstacles;
    int quit_time;
    int proximity_threshold;
    int max_speed_level_1;
    int max_speed_level_2;
    int max_speed_level_3;
    short car_color;
    short friendly_car_color;
    short frog_color;
    short coin_color;
    short goal_color;
    short stork_color;
    short road_color;
} Config;

// Function declaration for loading configuration from file.
void load_config(const char *filename, Config *config);

#endif
