#include "game.h"
#include <string.h>
#define COLOR_GREY 8
// Function to save the game state to a file
void save_game(GameState *game_state, const char *filename) {
    FILE *file = fopen(filename, "wb"); // Open the file in binary write mode
    if (file == NULL) {
        perror("Error opening file for saving");
        return;
    }
    fwrite(game_state, sizeof(GameState), 1, file); // Write the game state to the file
    fclose(file); // Close the file
}

// Function to load the game state from a file
void load_game(GameState *game_state, const char *filename) {
    FILE *file = fopen(filename, "rb"); // Open the file in binary read mode
    if (file == NULL) {
        perror("Error opening file for loading");
        return;
    }
    fread(game_state, sizeof(GameState), 1, file); // Read the game state from the file
    fclose(file); // Close the file
}

// Function to display the current level with save/load prompts
void display_level(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->frog_color));
    mvprintw(0, 0, "Press 'q' to save"); // Save message on the left
    mvprintw(0, config->screen_width / 2 - 6, "Level: %d", game_state->level); // Display level
    mvprintw(0, config->screen_width - 17, "Press 'l' to load"); // Load message on the right
    attroff(COLOR_PAIR(config->frog_color));
}

// Initialize ncurses screen and check for errors
WINDOW* init_screen() {
    WINDOW* win;
    if ((win = initscr()) == NULL) { 
        fprintf(stderr, "Error initializing ncurses.\n");
        exit(EXIT_FAILURE);
    }
    return win;
}

// Check if the terminal supports colors
void check_colors_support() {
    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Your terminal does not support color\n");
        exit(EXIT_FAILURE);
    }
}

// Initialize color pairs for the game
void init_colors(Config *config) {
    start_color(); // Start color functionality
    init_color(COLOR_GREY, 700, 700, 700);
    init_pair(config->car_color, COLOR_RED, COLOR_BLACK); // Initialize car color
    init_pair(config->friendly_car_color, COLOR_BLUE, COLOR_BLACK); // Initialize friendly car color
    init_pair(config->frog_color, COLOR_GREEN, COLOR_BLACK); // Initialize frog color
    init_pair(config->road_color, COLOR_BLACK, COLOR_BLACK); // Initialize road color
    init_pair(config->coin_color, COLOR_YELLOW, COLOR_BLACK); // Initialize coin color
    init_pair(config->goal_color, COLOR_BLUE, COLOR_BLACK); // Initialize goal color
    init_pair(config->stork_color, COLOR_MAGENTA, COLOR_BLACK); // Initialize stork color
    init_pair(config->obstacles_color, COLOR_GREY, COLOR_BLACK); // Initialize obstacles color
    init_pair(config->lane_color, COLOR_WHITE, COLOR_BLACK);// Initialize lane color
}

// Set ncurses options for the game
void set_ncurses_options() {
    noecho(); // Don't echo keystrokes
    curs_set(0); // Hide cursor
}

// Function to initialize and start the game window
WINDOW* Start(Config *config) {
    WINDOW* win = init_screen(); // Initialize screen
    check_colors_support(); // Check color support
    init_colors(config); // Initialize colors
    set_ncurses_options(); // Set ncurses options
    return win; // Return the window pointer
}


// Function to display the welcome screen
void Welcome(WINDOW *win) {
    int mid_x = COLS / 2;
    int mid_y = LINES / 2;

    attron(COLOR_PAIR(3)); 
    mvwaddstr(win, mid_y - 2, mid_x - 6, "Jumping Frog"); // Title
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(4));
    mvwaddstr(win, mid_y - 1, mid_x - 10, "Press any key to play"); 
    attroff(COLOR_PAIR(4));
    mvwaddstr(win, LINES - 4, COLS - 12, "Created by:");
    mvwaddstr(win, LINES - 3, COLS - 21, "Maksym Berezhnytskyi"); 
    mvwaddstr(win, LINES - 2, COLS - 22, "Student index: 206458");  
    wgetch(win); // Wait for key press
    wclear(win); // Clear window
    wrefresh(win); // Refresh window
}

// Function to draw the frog character
void draw_frog(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->frog_color));
    mvaddch(game_state->frog_y, game_state->frog_x, (unsigned int)config->frog_shape); // Draw frog at its current position
    attroff(COLOR_PAIR(config->frog_color));
}

// Function to draw the road stripes
void draw_road_stripes(int screen_height, int screen_width) {
    for (int y = 2; y < screen_height - 2; y++) {
        if (y % 2 == 0) {
            for (int x = 0; x < screen_width; x++) {
                mvaddch(y, x, '|'); // Draw stripe
            }
        }
    }
}

// Function to draw the road background
void draw_road(int screen_height, int screen_width, Config *config) {
    for (int y = 2; y < screen_height - 2; y++) {
        for (int x = 0; x < screen_width; x++) {
            attron(COLOR_PAIR(config->road_color));
            mvaddch(y, x, ' '); // Draw road background
            attroff(COLOR_PAIR(config->road_color));
        }
    }
    draw_road_stripes(screen_height, screen_width); // Draw road stripes
}


// Function to draw the goal area
void draw_goal(int screen_width, Config *config) {
    attron(COLOR_PAIR(config->goal_color));
    for (int x = 0; x < screen_width; x++) {
        mvaddch(1, x, 'G'); // Draw goal area
    }
    attroff(COLOR_PAIR(config->goal_color));
}

// Function to draw cars on the road
void draw_cars(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->car_color));
    for (int i = 0; i < MAX_CARS; i++) {
        if (game_state->car_spawn_delay[i] == 0) { // Check if car should be visible
            mvaddch(game_state->cars_y[i], game_state->cars_x[i], (unsigned int)config->car_shape); // Draw car
        }
    }
    attroff(COLOR_PAIR(config->car_color));
}

// Function to draw friendly cars that help the frog
void draw_friendly_cars(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->friendly_car_color));
    for (int i = 0; i < MAX_FRIENDLY_CARS; i++) {
        mvaddch(game_state->friendly_cars_y[i], game_state->friendly_cars_x[i], (unsigned int)config->friendly_car_shape); // Draw friendly car
    }
    attroff(COLOR_PAIR(config->friendly_car_color));
}

// Function to draw coins that the frog can collect
void draw_coins(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->coin_color));
    for (int i = 0; i < MAX_COINS; i++) {
        if (!game_state->coins_collected[i]) {
            mvaddch(game_state->coins_y[i], game_state->coins_x[i], 'O'); // Draw coin
        }
    }
    attroff(COLOR_PAIR(config->coin_color));
}

// Function to draw obstacles on the road
void draw_obstacles(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->obstacles_color));
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        for (int j = 0; j < 3; j++) {
            mvaddch(game_state->obstacles_y[i], game_state->obstacles_x[i] + j, 'X'); // Draw obstacle
        }
    }
    attroff(COLOR_PAIR(config->obstacles_color));
}

// Function to draw the stork character
void draw_stork(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->stork_color));
    mvaddch(game_state->stork_y, game_state->stork_x, (unsigned int)config->stork_shape); // Draw stork
    attroff(COLOR_PAIR(config->stork_color));
}

// Function to get time since the last frog jump
double get_time_since_last_jump(time_t last_jump_time) {
    return difftime(time(NULL), last_jump_time); // Calculate difference in time
}

// Function to move the frog to a new position
void move_frog_position(GameState *game_state, Config *config, int dx, int dy) {
    // Ensure new position is within screen boundaries
    if (game_state->frog_x + dx >= 0 && game_state->frog_x + dx < config->screen_width &&
        game_state->frog_y + dy >= 0 && game_state->frog_y + dy < config->screen_height) {
        
        // Check for barriers
        game_state->frog_x += dx;
        game_state->frog_y += dy;
        if (check_collision(game_state, config) == 2) {
            game_state->frog_x -= dx; // Revert if a barrier is encountered
            game_state->frog_y -= dy; // Revert if a barrier is encountered
        }
    }
}

// Function to move the stork closer to the frog
void move_stork(GameState *game_state) {
    int stork_dx = (game_state->frog_x > game_state->stork_x) ? 1 : ((game_state->frog_x < game_state->stork_x) ? -1 : 0);
    int stork_dy = (game_state->frog_y > game_state->stork_y) ? 1 : ((game_state->frog_y < game_state->stork_y) ? -1 : 0);

    game_state->stork_x += stork_dx; // Move stork horizontally
    game_state->stork_y += stork_dy; // Move stork vertically
}

// Function to update the frog's carrying status
void update_frog_carrying_status(GameState *game_state) {
    game_state->frog_carried = 0; // Frog is not being carried
    game_state->carrying_car_index = -1; // Reset carrying car index
}

// Function to update the last jump time and step count
void update_jump_time_and_steps(GameState *game_state) {
    game_state->last_jump_time = time(NULL); // Update the last jump time
    game_state->frog_steps++; // Increment frog's step count
}

// Function to move the frog and handle carrying status and stork movements
void move_frog(GameState *game_state, Config *config, int dx, int dy) {
    if (get_time_since_last_jump(game_state->last_jump_time) >= 1.0) {
        if (game_state->frog_carried) { // If frog is being carried, update status
            update_frog_carrying_status(game_state);
        } else {
            move_frog_position(game_state, config, dx, dy); // Move frog
        }
        update_jump_time_and_steps(game_state);

        // Move stork if conditions are met
        if (game_state->level >= 2) {
            if ((game_state->level == 2 && game_state->frog_steps % 2 == 0) ||
                (game_state->level >= 3 && game_state->frog_steps % 1 == 0)) {
                move_stork(game_state);
            }
        }
    }
}

// Update position and direction of a single friendly car
void update_single_friendly_car(GameState *game_state, Config *config, int i) {
    game_state->friendly_cars_x[i] += (short int)(game_state->friendly_cars_direction[i] * game_state->friendly_car_speed[i]);
    if (game_state->friendly_cars_x[i] >= config->screen_width) { // If car reaches the screen boundary
        game_state->friendly_cars_direction[i] = -1; // Change direction
        game_state->friendly_cars_x[i] = (short int)(config->screen_width - 1);
    } else if (game_state->friendly_cars_x[i] < 0) {
        game_state->friendly_cars_direction[i] = 1; // Change direction
        game_state->friendly_cars_x[i] = 0;
    }
}

// Check if frog is carried by friendly car
void check_frog_carried(GameState *game_state, int i) {
    if (game_state->frog_x == game_state->friendly_cars_x[i] && game_state->frog_y == game_state->friendly_cars_y[i]) {
        game_state->frog_carried = 1;
        game_state->carrying_car_index = i;
    }
}

// Function to update the positions of friendly cars
void update_friendly_cars(GameState *game_state, Config *config) {
    for (int i = 0; i < config->max_friendly_cars; i++) {
        update_single_friendly_car(game_state, config, i); // Update position and direction of friendly car
        check_frog_carried(game_state, i); // Check if frog is on the friendly car
    }
}

// Function to update the frog's position if it's being carried
void update_frog(GameState *game_state) {
    if (game_state->frog_carried) {
        game_state->frog_x = game_state->friendly_cars_x[game_state->carrying_car_index];
        game_state->frog_y = game_state->friendly_cars_y[game_state->carrying_car_index];
    }
}

// Function to move a car based on its speed and direction
void move_car(GameState *game_state, int i) {
    game_state->cars_x[i] += (short int)(game_state->cars_direction[i] * game_state->car_speed[i]);
}

// Function to update car direction and position based on screen boundaries
void update_car_direction(GameState *game_state, Config *config, int i) {
    if (i < 5) { // Logic for first 5 cars
        if (game_state->cars_x[i] >= config->screen_width) {
            game_state->cars_direction[i] = -1; // Change direction to left
            game_state->cars_x[i] = (short int)(config->screen_width - 1);
        } else if (game_state->cars_x[i] < 0) {
            game_state->cars_direction[i] = 1; // Change direction to right
            game_state->cars_x[i] = 0;
        }
    } else { // Logic for other cars
        if (game_state->cars_x[i] >= config->screen_width) {
            game_state->cars_x[i] = 0;
            game_state->car_spawn_delay[i] = rand() % 10 + 1; // Random spawn delay
        } else if (game_state->cars_x[i] < 0) {
            game_state->cars_x[i] = (short int)(config->screen_width - 1);
            game_state->car_spawn_delay[i] = rand() % 10 + 1; // Random spawn delay
        }
    }
}

// Function to handle car spawn delays
void handle_car_spawn_delay(GameState *game_state, int i) {
    if (game_state->car_spawn_delay[i] > 0) {
        game_state->car_spawn_delay[i]--;
    }
}

// Function to update enemy cars, including their movement and direction
void update_enemy_cars(GameState *game_state, Config *config) {
    for (int i = 0; i < config->max_cars; i++) {
        handle_car_spawn_delay(game_state, i);

        if (game_state->car_spawn_delay[i] == 0) {
            // Logic for stopping cars if frog is near
            if (game_state->stopping_cars[i] && abs(game_state->frog_x - game_state->cars_x[i]) + abs(game_state->frog_y - game_state->cars_y[i]) <= config->proximity_threshold) {
                game_state->car_speed[i] = 0;
            } else {
                // Adjust car speed at random intervals
                if (i % 2 == 0 && rand() % 10 < 1) {
                    game_state->car_speed[i] = (short int)((rand() % config->max_speed_level_3) + 1);
                }
                move_car(game_state, i); // Move the car
                update_car_direction(game_state, config, i); // Update its direction
            }
        }
    }
}

// Function to update the game state by updating cars and frog
void update_game(GameState *game_state, Config *config) {
    update_friendly_cars(game_state, config);
    update_frog(game_state);
    update_enemy_cars(game_state, config);
}

// Checks for collision with cars
int check_car_collision(GameState *game_state, Config *config) {
    for (int i = 0; i < config->max_cars; i++) {
        int hitbox_size = game_state->car_speed[i];
        for (int j = -hitbox_size; j <= hitbox_size; j++) {
            if (game_state->frog_x == game_state->cars_x[i] + j && game_state->frog_y == game_state->cars_y[i]) {
                return 1; // Collision detected
            }
        }
    }
    return 0;
}

// Checks for collision with obstacles
int check_obstacle_collision(GameState *game_state) {
    for (int i = 0; i < game_state->num_obstacles; i++) {
        for (int j = 0; j < 3; j++) {
            if (game_state->frog_x == game_state->obstacles_x[i] + j && game_state->frog_y == game_state->obstacles_y[i]) {
                return 2; // Encountered a barrier
            }
        }
    }
    return 0;
}

// Checks for collision with stork if level is 2 or higher
int check_stork_collision(GameState *game_state) {
    if (game_state->level >= 2) {
        if (game_state->frog_x == game_state->stork_x && game_state->frog_y == game_state->stork_y) {
            return 1; // Collision detected
        }
    }
    return 0;
}

// Main function to check for collisions with cars, obstacles, and stork
int check_collision(GameState *game_state, Config *config) {
    if (game_state->frog_carried) {
        return 0; // No collision if frog is being carried
    }

    int collision_result;

    collision_result = check_car_collision(game_state, config);
    if (collision_result != 0) {
        return collision_result;
    }

    collision_result = check_obstacle_collision(game_state);
    if (collision_result != 0) {
        return collision_result;
    }

    collision_result = check_stork_collision(game_state);
    if (collision_result != 0) {
        return collision_result;
    }

    return 0;
}



// Function to check if the frog has collected any coins
void check_coin_collection(GameState *game_state, Config *config) {
    for (int i = 0; i < config->max_coins; i++) {
        if (game_state->frog_x == game_state->coins_x[i] && game_state->frog_y == game_state->coins_y[i] && !game_state->coins_collected[i]) {
            game_state->score++;
            game_state->coins_collected[i] = 1; // Mark coin as collected
        }
    }
}

// Function to generate coins in random positions
void generate_coins(GameState *game_state, Config *config) {
    srand((unsigned int)time(NULL));
    for (int i = 0; i < config->max_coins; i++) {
        game_state->coins_x[i] = rand() % config->screen_width;
        game_state->coins_y[i] = rand() % (config->screen_height - 4) + 2; // Avoid top and bottom rows
        game_state->coins_collected[i] = 0;

        // Ensure no two coins have the same y position
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
    int num_obstacles = (config->screen_width * config->screen_height) / 200; // Number of obstacles based on screen size
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

// Function to initialize the frog's starting position
void initialize_frog(GameState *game_state, Config *config) {
    game_state->frog_x = config->screen_width / 2; // Start frog in the middle
    game_state->frog_y = config->screen_height - 2; // Start frog at the bottom
}

// Function to initialize the positions and properties of enemy cars
void initialize_cars(GameState *game_state, Config *config) {
    srand((unsigned int)time(NULL));
    for (int i = 0; i < config->max_cars; i++) {
        game_state->cars_x[i] = rand() % config->screen_width;
        game_state->cars_y[i] = 2 + i * 2; // Position cars on different rows
        game_state->cars_direction[i] = (short int)((rand() % 2 == 0) ? 1 : -1); // Randomize car direction
        int max_speed = (game_state->level == 1) ? config->max_speed_level_1 : (game_state->level == 2) ? config->max_speed_level_2 : config->max_speed_level_3;
        game_state->car_speed[i] = (short int)(rand() % max_speed + 1); // Randomize car speed based on level
        game_state->car_spawn_delay[i] = rand() % 10 + 1; // Randomize spawn delay
    }
}

// Function to initialize the positions and properties of friendly cars
void initialize_friendly_cars(GameState *game_state, Config *config) {
    for (int i = 0; i < config->max_friendly_cars; i++) {
        game_state->friendly_cars_x[i] = rand() % config->screen_width;
        game_state->friendly_cars_y[i] = 2 + (i * 2) % (config->screen_height - 4); // Avoid top and bottom rows
        game_state->friendly_cars_direction[i] = (short int)((rand() % 2 == 0) ? 1 : -1); // Randomize direction
        game_state->friendly_car_speed[i] = (short int)(rand() % config->max_speed_level_1 + 1); // Randomize speed
    }
}

// Function to initialize which cars will stop if frog is near
void initialize_stopping_cars(GameState *game_state, Config *config) {
    for (int i = 0; i < config->max_cars; i++) {
        game_state->stopping_cars[i] = 0;
    }
    int stopping_car_count = 0;
    while (stopping_car_count < 2) { // Ensure there are two stopping cars
        int car_index = rand() % config->max_cars;
        if (!game_state->stopping_cars[car_index]) {
            game_state->stopping_cars[car_index] = 1;
            stopping_car_count++;
        }
    }
}

// Function to initialize the stork's starting position
void initialize_stork(GameState *game_state, Config *config) {
    if (game_state->level >= 2) {
        game_state->stork_x = (short int)(config->screen_width - 1); // Position stork at the right edge
        game_state->stork_y = (short int)(config->screen_height - 2); // Position stork at the bottom
    }
}

// Function to restart the game by reinitializing all elements
void restart_game(GameState *game_state, Config *config) {
    initialize_frog(game_state, config);
    initialize_cars(game_state, config);
    initialize_friendly_cars(game_state, config);
    initialize_stopping_cars(game_state, config);
    initialize_stork(game_state, config);
    generate_coins(game_state, config);
    generate_obstacles(game_state, config);
    game_state->frog_steps = 0; // Reset frog steps
}

// Function to progress to the next level or end the game if the max level is reached
void next_level(GameState *game_state, Config *config) {
    if (game_state->level < 3) {
        game_state->level++;
        restart_game(game_state, config);
    } else {
        game_state->lives = 0; // End the game if max level is reached
    }
}

// Function to display the current score
void display_score(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->frog_color));
    mvprintw(LINES - 4, 2, "Score: %d", game_state->score); // Show score at the bottom
    attroff(COLOR_PAIR(config->frog_color));
}

// Function to display the remaining lives
void display_lives(GameState *game_state, Config *config) {
    attron(COLOR_PAIR(config->frog_color));
    mvprintw(LINES - 3, 2, "Lives: %d", game_state->lives); // Show lives at the bottom
    attroff(COLOR_PAIR(config->frog_color));
}

// Function to display the elapsed time
void display_timer(GameState *game_state, Config *config) {
    time_t current_time = time(NULL);
    double elapsed_time = difftime(current_time, game_state->start_time); // Calculate elapsed time
    attron(COLOR_PAIR(config->frog_color));
    mvprintw(config->screen_height - 2, 2, "Time: %.0f seconds", elapsed_time); // Show time at the bottom
    attroff(COLOR_PAIR(config->frog_color));
}

// Function to show the end game screen with the final score and playtime
void EndGame(const char* info, Config *config) {
    clear();
    box(stdscr, 0, 0);
    attron(A_BOLD);
    mvprintw(LINES / 2 - 3, (COLS - (int)strlen("GAME OVER")) / 2, "GAME OVER"); // Display game over message
    attroff(A_BOLD);
    mvprintw(LINES / 2 - 1, (COLS - (int)strlen(info)) / 2, "%s", info); // Display final score and time played
    int remaining_time = config->quit_time; // Time before automatic exit
    while (remaining_time > 0) {
        mvprintw(LINES / 2 + 1, (COLS - 39) / 2, "Press any key to exit or wait %d seconds...", remaining_time);
        refresh();
        timeout(1000); // Wait for 1 second
        if (getch() != ERR) {
            break;
        }
        remaining_time--;
    }
}

// Function to handle the game over state, showing final score and playtime
void display_game_over(GameState *game_state, Config *config) {
    time_t end_time = time(NULL);
    double elapsed_time = difftime(end_time, game_state->start_time); // Calculate total playtime
    char info[100];
    sprintf(info, "Final Score: %d | Time Played: %.2f seconds", game_state->score, elapsed_time);
    EndGame(info, config); // Show end game screen with final score and time
}
