#include "game.h"
#include <string.h>

// Function to save the game state to a file
void save_game(GameState *game_state, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file for saving");
        return;
    }
    fwrite(game_state, sizeof(GameState), 1, file);
    fclose(file);
}

// Function to load the game state from a file
void load_game(GameState *game_state, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file for loading");
        return;
    }
    fread(game_state, sizeof(GameState), 1, file);
    fclose(file);
}

// Function to display the current level with save/load prompts
void display_level(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->frog_color));
    mvprintw(0, 0, "Press 'q' to save"); // Save message on the left
    mvprintw(0, config->screen_width / 2 - 6, "Level: %d", game_state->level); // Display level
    mvprintw(0, config->screen_width - 17, "Press 'l' to load"); // Load message on the right
    attroff(COLOR_PAIR(config->frog_color));
}

// Function to initialize and start the game window
WINDOW* Start(Config *config) {
    WINDOW* win;
    if ((win = initscr()) == NULL) {
        fprintf(stderr, "Error initializing ncurses.\n");
        exit(EXIT_FAILURE);
    }
    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Your terminal does not support color\n");
        exit(EXIT_FAILURE);
    }
    start_color();
    init_pair(config->car_color, COLOR_RED, COLOR_BLACK);
    init_pair(config->friendly_car_color, COLOR_BLUE, COLOR_BLACK);
    init_pair(config->frog_color, COLOR_GREEN, COLOR_BLACK);
    init_pair(config->road_color, COLOR_BLACK, COLOR_BLACK);
    init_pair(config->coin_color, COLOR_YELLOW, COLOR_BLACK);
    init_pair(config->goal_color, COLOR_BLUE, COLOR_BLACK);
    init_pair(config->stork_color, COLOR_MAGENTA, COLOR_BLACK);
    noecho();
    curs_set(0);
    return win;
}

// Function to display the welcome screen
void Welcome(WINDOW *win) {
    int mid_x = COLS / 2;
    int mid_y = LINES / 2;

    attron(COLOR_PAIR(3)); 
    mvwaddstr(win, mid_y - 2, mid_x - 6, "Jumping Frog"); 
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(4));
    mvwaddstr(win, mid_y - 1, mid_x - 10, "Press any key to play"); 
    attroff(COLOR_PAIR(4));
    mvwaddstr(win, LINES - 4, COLS - 12, "Created by:");
    mvwaddstr(win, LINES - 3, COLS - 21, "Maksym Berezhnytskyi"); 
    mvwaddstr(win, LINES - 2, COLS - 22, "Student index: 206458");  
    wgetch(win);
    wclear(win);
    wrefresh(win);
}

// Function to draw the frog character
void draw_frog(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->frog_color));
    mvaddch(game_state->frog_y, game_state->frog_x, (unsigned int)config->frog_shape);
    attroff(COLOR_PAIR(config->frog_color));
}

// Function to draw the road background
void draw_road(int screen_height, int screen_width, Config *config) {
    for (int y = 2; y < screen_height - 2; y++) {
        for (int x = 0; x < screen_width; x++) {
            attron(COLOR_PAIR(config->road_color));
            mvaddch(y, x, ' ');
            attroff(COLOR_PAIR(config->road_color));
        }
    }
}

// Function to draw the goal area
void draw_goal(int screen_width, Config *config) {
    attron(COLOR_PAIR(config->goal_color));
    for (int x = 0; x < screen_width; x++) {
        mvaddch(1, x, 'G');
    }
    attroff(COLOR_PAIR(config->goal_color));
}

// Function to draw cars on the road
void draw_cars(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->car_color));
    for (int i = 0; i < MAX_CARS; i++) {
        if (game_state->car_spawn_delay[i] == 0) {
            mvaddch(game_state->cars_y[i], game_state->cars_x[i], (unsigned int)config->car_shape);
        }
    }
    attroff(COLOR_PAIR(config->car_color));
}

// Function to draw friendly cars that help the frog
void draw_friendly_cars(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->friendly_car_color));
    for (int i = 0; i < MAX_FRIENDLY_CARS; i++) {
        mvaddch(game_state->friendly_cars_y[i], game_state->friendly_cars_x[i], (unsigned int)config->friendly_car_shape);
    }
    attroff(COLOR_PAIR(config->friendly_car_color));
}

// Function to draw coins that the frog can collect
void draw_coins(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->coin_color));
    for (int i = 0; i < MAX_COINS; i++) {
        if (!game_state->coins_collected[i]) {
            mvaddch(game_state->coins_y[i], game_state->coins_x[i], 'O');
        }
    }
    attroff(COLOR_PAIR(config->coin_color));
}

// Function to draw obstacles on the road
void draw_obstacles(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->road_color));
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        for (int j = 0; j < 3; j++) {
            mvaddch(game_state->obstacles_y[i], game_state->obstacles_x[i] + j, 'X');
        }
    }
    attroff(COLOR_PAIR(config->road_color));
}

// Function to draw the stork character
void draw_stork(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->stork_color));
    mvaddch(game_state->stork_y, game_state->stork_x, (unsigned int)config->stork_shape);
    attroff(COLOR_PAIR(config->stork_color));
}

// Function to move the frog character
void move_frog(GameState *game_state, Config *config, int dx, int dy) {
    time_t current_time = time(NULL);
    double time_since_last_jump = difftime(current_time, game_state->last_jump_time);
    if (time_since_last_jump >= 1.0) {
        if (game_state->frog_carried) {
            game_state->frog_carried = 0;
            game_state->carrying_car_index = -1;
        } else {
            if (game_state->frog_x + dx >= 0 && game_state->frog_x + dx < config->screen_width)
                game_state->frog_x += dx;
            if (game_state->frog_y + dy >= 0 && game_state->frog_y + dy < config->screen_height)
                game_state->frog_y += dy;
        }
        game_state->last_jump_time = current_time;

        game_state->frog_steps++;

        if (game_state->level == 2 && game_state->frog_steps % 2 == 0) {
            int stork_dx = (game_state->frog_x > game_state->stork_x) ? 1 : ((game_state->frog_x < game_state->stork_x) ? -1 : 0);
            int stork_dy = (game_state->frog_y > game_state->stork_y) ? 1 : ((game_state->frog_y < game_state->stork_y) ? -1 : 0);

            game_state->stork_x += stork_dx;
            game_state->stork_y += stork_dy;
        } else if (game_state->level >= 3 && game_state->frog_steps % 1 == 0) {
            int stork_dx = (game_state->frog_x > game_state->stork_x) ? 1 : ((game_state->frog_x < game_state->stork_x) ? -1 : 0);
            int stork_dy = (game_state->frog_y > game_state->stork_y) ? 1 : ((game_state->frog_y < game_state->stork_y) ? -1 : 0);

            game_state->stork_x += (short int)stork_dx;
            game_state->stork_y += (short int)stork_dy;
        }
    }
}

// Function to update the game state, including positions of cars and frog's interactions with cars
void update_game(GameState *game_state, Config *config) {
    for (int i = 0; i < config->max_friendly_cars; i++) {
        game_state->friendly_cars_x[i] += (short int)(game_state->friendly_cars_direction[i] * game_state->friendly_car_speed[i]);
        if (game_state->friendly_cars_x[i] >= config->screen_width) {
            game_state->friendly_cars_direction[i] = -1;
            game_state->friendly_cars_x[i] = (short int)(config->screen_width - 1);
        } else if (game_state->friendly_cars_x[i] < 0) {
            game_state->friendly_cars_direction[i] = 1;
            game_state->friendly_cars_x[i] = 0;
        }

        if (game_state->frog_x == game_state->friendly_cars_x[i] && game_state->frog_y == game_state->friendly_cars_y[i]) {
            game_state->frog_carried = 1;
            game_state->carrying_car_index = i;
        }
    }

    if (game_state->frog_carried) {
        game_state->frog_x = game_state->friendly_cars_x[game_state->carrying_car_index];
        game_state->frog_y = game_state->friendly_cars_y[game_state->carrying_car_index];
    }

    for (int i = 0; i < config->max_cars; i++) {
        if (game_state->car_spawn_delay[i] > 0) {
            game_state->car_spawn_delay[i]--;
        } else {
            if (game_state->stopping_cars[i] && abs(game_state->frog_x - game_state->cars_x[i]) + abs(game_state->frog_y - game_state->cars_y[i]) <= config->proximity_threshold) {
                game_state->car_speed[i] = 0;
            } else {
                if (i % 2 == 0 && rand() % 10 < 1) {
                    game_state->car_speed[i] = (short int)((rand() % config->max_speed_level_3) + 1);
                }
                game_state->cars_x[i] += (short int)(game_state->cars_direction[i] * game_state->car_speed[i]);
                if (i < 5) {
                    if (game_state->cars_x[i] >= config->screen_width) {
                        game_state->cars_direction[i] = -1;
                        game_state->cars_x[i] = (short int)(config->screen_width - 1);
                    } else if (game_state->cars_x[i] < 0) {
                        game_state->cars_direction[i] = 1;
                        game_state->cars_x[i] = 0;
                    }
                } else {
                    if (game_state->cars_x[i] >= config->screen_width) {
                        game_state->cars_x[i] = 0;
                        game_state->car_spawn_delay[i] = rand() % 10 + 1;
                    } else if (game_state->cars_x[i] < 0) {
                        game_state->cars_x[i] = (short int)(config->screen_width - 1);
                        game_state->car_spawn_delay[i] = rand() % 10 + 1;
                    }
                }
            }
        }
    }
}

// Function to check if the frog has collided with any cars or obstacles
int check_collision(GameState *game_state, Config *config) {
    if (game_state->frog_carried) {
        return 0;
    }

    for (int i = 0; i < config->max_cars; i++) {
        int hitbox_size = game_state->car_speed[i];
        for (int j = -hitbox_size; j <= hitbox_size; j++) {
            if (game_state->frog_x == game_state->cars_x[i] + j && game_state->frog_y == game_state->cars_y[i]) {
                return 1;
            }
        }
    }

    for (int i = 0; i < game_state->num_obstacles; i++) {
        for (int j = 0; j < 3; j++) {
            if (game_state->frog_x == game_state->obstacles_x[i] + j && game_state->frog_y == game_state->obstacles_y[i]) {
                return 1;
            }
        }
    }

    if (game_state->level >= 2) {
        if (game_state->frog_x == game_state->stork_x && game_state->frog_y == game_state->stork_y) {
            return 1;
        }
    }
    return 0;
}

// Function to check if the frog has collected any coins
void check_coin_collection(GameState *game_state, Config *config) {
    for (int i = 0; i < config->max_coins; i++) {
        if (game_state->frog_x == game_state->coins_x[i] && game_state->frog_y == game_state->coins_y[i] && !game_state->coins_collected[i]) {
            game_state->score++;
            game_state->coins_collected[i] = 1;
        }
    }
}

// Function to generate coins in random positions
void generate_coins(GameState *game_state, Config *config) {
    srand((unsigned int)time(NULL));
    for (int i = 0; i < config->max_coins; i++) {
        game_state->coins_x[i] = rand() % config->screen_width;
        game_state->coins_y[i] = rand() % (config->screen_height - 4) + 2;
        game_state->coins_collected[i] = 0;
        for (int j = 0; j < i; j++) {
            while (game_state->coins_y[i] == game_state->coins_y[j]) {
                game_state->coins_y[i] = rand() % (config->screen_height - 4) + 2;
            }
        }
    }
}

// Function to generate obstacles in random positions
void generate_obstacles(GameState *game_state, Config *config) {
    srand((unsigned int)time(NULL));
    int num_obstacles = (config->screen_width * config->screen_height) / 200;
    game_state->num_obstacles = num_obstacles;

    for (int i = 0; i < num_obstacles; i++) {
        int valid_position = 0;

        while (!valid_position) {
            valid_position = 1;
            game_state->obstacles_x[i] = rand() % config->screen_width;
            game_state->obstacles_y[i] = rand() % (config->screen_height - 4) + 2;

            // Check if obstacle overlaps with a car
            for (int j = 0; j < config->max_cars; j++) {
                if (game_state->obstacles_y[i] == game_state->cars_y[j]) {
                    valid_position = 0;
                    break;
                }
            }
        }
    }
}

// Function to restart the game, reinitializing positions and states
void restart_game(GameState *game_state, Config *config) {
    game_state->frog_x = config->screen_width / 2;
    game_state->frog_y = config->screen_height - 2;
    srand((unsigned int)time(NULL));
    for (int i = 0; i < config->max_cars; i++) {
        game_state->cars_x[i] = rand() % config->screen_width;
        game_state->cars_y[i] = 2 + i * 2;
        game_state->cars_direction[i] = (short int)((rand() % 2 == 0) ? 1 : -1);
        int max_speed = (game_state->level == 1) ? config->max_speed_level_1 : (game_state->level == 2) ? config->max_speed_level_2 : config->max_speed_level_3;
        game_state->car_speed[i] = (short int)(rand() % max_speed + 1);
        game_state->car_spawn_delay[i] = rand() % 10 + 1;
    }
    for (int i = 0; i < config->max_friendly_cars; i++) {
        game_state->friendly_cars_x[i] = rand() % config->screen_width;
        game_state->friendly_cars_y[i] = 2 + (i * 2) % (config->screen_height - 4);
        game_state->friendly_cars_direction[i] = (short int)((rand() % 2 == 0) ? 1 : -1);
        game_state->friendly_car_speed[i] = (short int)(rand() % config->max_speed_level_1 + 1);
    }
    for (int i = 0; i < config->max_cars; i++) {
        game_state->stopping_cars[i] = 0;
    }
    int stopping_car_count = 0;
    while (stopping_car_count < 2) {
        int car_index = rand() % config->max_cars;
        if (!game_state->stopping_cars[car_index]) {
            game_state->stopping_cars[car_index] = 1;
            stopping_car_count++;
        }
    }
    generate_coins(game_state, config);
    generate_obstacles(game_state, config);

    if (game_state->level >= 2) {
        game_state->stork_x = (short int)(config->screen_width - 1);
        game_state->stork_y = (short int)(config->screen_height - 2);
    }
    game_state->frog_steps = 0;
}

// Function to progress to the next level or end the game if the max level is reached
void next_level(GameState *game_state, Config *config) {
    if (game_state->level < 3) {
        game_state->level++;
        restart_game(game_state, config);
    } else {
        game_state->lives = 0;
    }
}

// Function to display the current score
void display_score(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->frog_color));
    mvprintw(LINES - 4, 2, "Score: %d", game_state->score);
    attroff(COLOR_PAIR(config->frog_color));
}

// Function to display the remaining lives
void display_lives(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->frog_color));
    mvprintw(LINES - 3, 2, "Lives: %d", game_state->lives);
    attroff(COLOR_PAIR(config->frog_color));
}

// Function to display the elapsed time
void display_timer(GameState *game_state, Config *config) {
    time_t current_time = time(NULL);
    double elapsed_time = difftime(current_time, game_state->start_time);
    attron(COLOR_PAIR(config->frog_color));
    mvprintw(config->screen_height - 2, 2, "Time: %.0f seconds", elapsed_time);
    attroff(COLOR_PAIR(config->frog_color));
}

// Function to show the end game screen with the final score and playtime
void EndGame(const char* info, Config *config) {
    clear();
    box(stdscr, 0, 0);
    attron(A_BOLD);
    mvprintw(LINES / 2 - 3, (COLS - (int)strlen("GAME OVER")) / 2, "GAME OVER");
    attroff(A_BOLD);
    mvprintw(LINES / 2 - 1, (COLS - (int)strlen(info)) / 2, "%s", info);
    int remaining_time = config->quit_time;
    while (remaining_time > 0) {
        mvprintw(LINES / 2 + 1, (COLS - 39) / 2, "Press any key to exit or wait %d seconds...", remaining_time);
        refresh();
        timeout(1000);
        if (getch() != ERR) {
            break;
        }
        remaining_time--;
    }
}

// Function to handle the game over state, showing final score and playtime
void display_game_over(GameState *game_state, Config *config) {
    time_t end_time = time(NULL);
    double elapsed_time = difftime(end_time, game_state->start_time);
    char info[100];
    sprintf(info, "Final Score: %d | Time Played: %.2f seconds", game_state->score, elapsed_time);
    EndGame(info, config);
}
