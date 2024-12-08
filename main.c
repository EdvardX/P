// Some fragments of this code were taken from [Demo game - Catch the Ball].

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "config.h"
#include "game.h"

int main() {
    GameState game_state = {0};
    Config config;

    // Loading configuration from file
    load_config("config.txt", &config);

    // Initializing game window
    WINDOW* mainwin = Start(&config);
    Welcome(mainwin);
    initscr();
    noecho();
    curs_set(0);
    timeout(100);
    keypad(stdscr, TRUE);

    // Checking color support in terminal
    if (!has_colors()) {
        endwin();
        printf("Terminal does not support colors.\n");
        return 1;
    }

    // Setting screen dimensions
    int screen_width, screen_height;
    getmaxyx(stdscr, screen_height, screen_width);
    config.screen_width = screen_width;
    config.screen_height = screen_height;

    // Initializing game state
    game_state.level = 1;
    game_state.lives = 3;
    game_state.start_time = time(NULL);
    game_state.last_jump_time = time(NULL);

    // Main game loop
    while (game_state.lives > 0) {
        restart_game(&game_state, &config);

        while (1) {
            clear();
            getmaxyx(stdscr, screen_height, screen_width);
            
            // Drawing game elements
            draw_road(screen_height, screen_width, &config);
            draw_goal(screen_width, &config);
            draw_frog(&game_state, &config);
            draw_cars(&game_state, &config);
            draw_friendly_cars(&game_state, &config);
            draw_coins(&game_state, &config);
            draw_obstacles(&game_state, &config);
            if (game_state.level >= 2) {
                draw_stork(&game_state, &config);
            }

            // Displaying game information
            display_level(&game_state, &config);
            display_score(&game_state, &config);
            display_lives(&game_state, &config);
            display_timer(&game_state, &config);

            // Updating game state
            update_game(&game_state, &config);

            // Checking coin collection
            check_coin_collection(&game_state, &config);

            // Checking collisions
            if (check_collision(&game_state, &config)) {
                game_state.lives--;
                game_state.frog_x = screen_width / 2;
                game_state.frog_y = screen_height - 2;
                if (game_state.lives == 0) {
                    break; // End game after losing all lives
                }
            }

            // Checking if the frog reached the goal
            if (game_state.frog_y == 1) {
                game_state.score += 5;
                next_level(&game_state, &config);
                break; // Proceed to the next level
            }

            // Handling user input
            int ch = getch();
            if (ch == 'q') {
                printf("Saving game...\n");
                save_game(&game_state, "savegame.dat");
                printf("Game saved.\n");
            
            } else if (ch == 'l') {
                printf("Loading game...\n");
                load_game(&game_state, "savegame.dat");
                printf("Game loaded.\n");
            } else if (ch == KEY_UP) {
                move_frog(&game_state, &config, 0, -1);
            } else if (ch == KEY_DOWN) {
                move_frog(&game_state, &config, 0, 1);
            } else if (ch == KEY_LEFT) {
                move_frog(&game_state, &config, -1, 0);
            } else if (ch == KEY_RIGHT) {
                move_frog(&game_state, &config, 1, 0);
            }
            refresh();
            usleep(1000); // Pause to reduce refresh rate
        }
    }
    // Displaying the game over screen
    display_game_over(&game_state, &config);
    endwin();
    return 0;
}
