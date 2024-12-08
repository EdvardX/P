#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "config.h"
#include "game.h"

// Initialize the game state and configuration settings
void init_game(GameState* game_state, Config* config) {
    int screen_width, screen_height;
    getmaxyx(stdscr, screen_height, screen_width);
    config->screen_width = screen_width;
    config->screen_height = screen_height;

    game_state->level = 1;
    game_state->lives = 3;
    game_state->start_time = time(NULL);
    game_state->last_jump_time = time(NULL);
}

// Function prototypes
void draw_game_elements(GameState* game_state, Config* config);
void check_game_events(GameState* game_state, Config* config);
void process_game_input(GameState* game_state, Config* config);

// The main game loop that handles the game progression and logic
void main_game_loop(GameState* game_state, Config* config) {
    while (game_state->lives > 0) {
        restart_game(game_state, config);

        while (1) {
            clear();
            getmaxyx(stdscr, config->screen_height, config->screen_width);
            draw_game_elements(game_state, config);
            check_game_events(game_state, config);
            process_game_input(game_state, config);

            if (game_state->lives == 0) {
                break; // End the loop if the player has no lives left
            }
            if (game_state->frog_y == 1) {
                game_state->score += 5;
                next_level(game_state, config); // Proceed to the next level
                break;
            }
            refresh();
            usleep(1000); // Pause to reduce refresh rate
        }
    }
}

// Draw all game elements on the screen
void draw_game_elements(GameState* game_state, Config* config) {
    draw_road(config->screen_height, config->screen_width, config);
    draw_goal(config->screen_width, config);
    draw_frog(game_state, config);
    draw_cars(game_state, config);
    draw_friendly_cars(game_state, config);
    draw_coins(game_state, config);
    draw_obstacles(game_state, config);
    if (game_state->level >= 2) {
    draw_stork(game_state, config);
    }
    display_level(game_state, config);
    display_score(game_state, config);
    display_lives(game_state, config);
    display_timer(game_state, config);
}

// Check for game events such as collisions and coin collections
void check_game_events(GameState* game_state, Config* config) {
    update_game(game_state, config);
    check_coin_collection(game_state, config);

    if (check_collision(game_state, config)) {
        game_state->lives--; // Reduce lives on collision
        game_state->frog_x = config->screen_width / 2; // Reset frog position
        game_state->frog_y = config->screen_height - 2;
    }
}

// Process input from the user to control the game
void process_game_input(GameState* game_state, Config* config) {
    int ch = getch();
    if (ch == 'q') {
        printf("Saving game...\n");
        save_game(game_state, "savegame.dat");
        printf("Game saved.\n");
    } else if (ch == 'l') {
        printf("Loading game...\n");
        load_game(game_state, "savegame.dat");
        printf("Game loaded.\n");
    } else if (ch == KEY_UP) {
        move_frog(game_state, config, 0, -1); // Move frog up
    } else if (ch == KEY_DOWN) {
        move_frog(game_state, config, 0, 1); // Move frog down
    } else if (ch == KEY_LEFT) {
        move_frog(game_state, config, -1, 0); // Move frog left
    } else if (ch == KEY_RIGHT) {
        move_frog(game_state, config, 1, 0); // Move frog right
    }
}

// Main function to start the game
int main() {
    GameState game_state = {0};
    Config config;

    load_config("config.txt", &config);
    WINDOW* mainwin = Start(&config);
    Welcome(mainwin);

    initscr();
    noecho();
    curs_set(0);
    timeout(100);
    keypad(stdscr, TRUE);

    if (!has_colors()) {
        endwin();
        printf("Terminal does not support colors.\n");
        return 1;
    }

    init_game(&game_state, &config);
    main_game_loop(&game_state, &config);
    
    display_game_over(&game_state, &config);
    endwin();
    return 0;
}
