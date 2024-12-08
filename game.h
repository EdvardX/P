#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include "config.h"

// GameState structure to store the game state, including positions of the frog, cars, coins, and obstacles
typedef struct GameState {
    // Frog position
    int frog_x, frog_y;
    
    // Positions, directions, and speeds of the cars
    int cars_x[MAX_CARS], cars_y[MAX_CARS];
    int cars_direction[MAX_CARS];
    int car_speed[MAX_CARS];
    int car_spawn_delay[MAX_CARS];
    
    // Positions, directions, and speeds of friendly cars
    int friendly_cars_x[MAX_FRIENDLY_CARS], friendly_cars_y[MAX_FRIENDLY_CARS];
    int friendly_cars_direction[MAX_FRIENDLY_CARS];
    int friendly_car_speed[MAX_FRIENDLY_CARS];
    
    // Positions and states of the coins
    int coins_x[MAX_COINS], coins_y[MAX_COINS];
    int coins_collected[MAX_COINS];
    
    // Positions and number of obstacles
    int obstacles_x[MAX_OBSTACLES], obstacles_y[MAX_OBSTACLES];
    int num_obstacles;
    
    // States of stopping cars
    int stopping_cars[MAX_CARS];
    
    // Information about the game level, score, and lives
    int level;
    int score;
    int lives;
    
    // Start time of the game and the time of the last jump
    time_t start_time;
    time_t last_jump_time;
    
    // Information about the frog being carried by the stork
    int frog_carried;
    int carrying_car_index;
    int stork_x, stork_y;
    int frog_steps;
} GameState;

// Function declarations
WINDOW* Start(Config *config);
void Welcome(WINDOW *win);
void draw_frog(GameState *game_state, Config *config);
void draw_road(int screen_height, int screen_width, Config *config);
void draw_goal(int screen_width, Config *config);
void draw_cars(GameState *game_state, Config *config);
void draw_friendly_cars(GameState *game_state, Config *config);
void draw_coins(GameState *game_state, Config *config);
void draw_obstacles(GameState *game_state, Config *config);
void draw_stork(GameState *game_state, Config *config);
void move_frog(GameState *game_state, Config *config, int dx, int dy);
void update_game(GameState *game_state, Config *config);
int check_collision(GameState *game_state, Config *config);
void check_coin_collection(GameState *game_state, Config *config);
void generate_coins(GameState *game_state, Config *config);
void generate_obstacles(GameState *game_state, Config *config);
void restart_game(GameState *game_state, Config *config);
void next_level(GameState *game_state, Config *config);
void display_level(GameState *game_state, Config *config);
void display_score(GameState *game_state, Config *config);
void display_lives(GameState *game_state, Config *config);
void display_timer(GameState *game_state, Config *config);
void EndGame(const char* info, Config *config);
void display_game_over(GameState *game_state, Config *config);

// Functions for saving and loading the game
void save_game(GameState *game_state, const char *filename);
void load_game(GameState *game_state, const char *filename);

#endif
