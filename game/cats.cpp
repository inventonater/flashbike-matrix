#include "system.h"
#include "renderer.h"
#include "game.h"

#include <stdlib.h>
#include <time.h>
#include <cmath>

const int GRID_WIDTH = 32;
const int GRID_HEIGHT = 24;
const int NUM_PLAYERS = 4;
const int NUM_MICE = 4;

typedef struct {
    int x, y;
} Position;

struct Cat {
    Position position;
    int size;
};

struct Mouse {
    Position position;
    bool alive;
};

struct GameState {
    Cat cats[NUM_PLAYERS];
    Mouse mice[NUM_MICE];
    bool game_over;
};

// Finds the best direction for the mouse to flee from the cats.
Position flee_from_cats(Position mouse_position, Position cat_positions[], int num_cats) {
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    double best_dist = -1;
    Position best_direction = {0, 0};

    for (int i = 0; i < 4; i++) {
        Position new_position = {mouse_position.x + dx[i], mouse_position.y + dy[i]};

        if (new_position.x < 0 || new_position.x >= GRID_WIDTH ||
            new_position.y < 0 || new_position.y >= GRID_HEIGHT) {
            continue;
        }

        double dist_sum = 0;
        for (int j = 0; j < num_cats; j++) {
            int dist_x = new_position.x - cat_positions[j].x;
            int dist_y = new_position.y - cat_positions[j].y;
            dist_sum += sqrt(dist_x * dist_x + dist_y * dist_y);
        }

        if (dist_sum > best_dist) {
            best_dist = dist_sum;
            best_direction = new_position;
        }
    }

    return best_direction;
}


void update_mice_positions(GameState *game_state) {
    for (int i = 0; i < NUM_MICE; i++) {
        if (!game_state->mice[i].alive) {
            continue;
        }

        Position cat_positions[NUM_PLAYERS];
        for (int j = 0; j < NUM_PLAYERS; j++) {
            cat_positions[j] = game_state->cats[j].position;
        }

        Position new_position = flee_from_cats(game_state->mice[i].position, cat_positions, NUM_PLAYERS);
        game_state->mice[i].position = new_position;
    }
}

bool is_mice_valid(const GameState *game_state, Position mice_position) {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        for (int j = 0; j < game_state->cats[i].size; j++) {
            if (game_state->cats[i].position.x == mice_position.x &&
                game_state->cats[i].position.y == mice_position.y) {
                return false;
            }
        }
    }
    return true;
}

void spawn_mice(GameState *game_state) {
    for (int i = 0; i < NUM_MICE; i++) {
        Position mice_position;
        do {
            mice_position.x = rand() % GRID_WIDTH;
            mice_position.y = rand() % GRID_HEIGHT;
        } while (!is_mice_valid(game_state, mice_position));
        game_state->mice[i].position = mice_position;
    }
}

void init_game(GameState *game_state) {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        game_state->cats[i].position.x = rand() % GRID_WIDTH;
        game_state->cats[i].position.y = rand() % GRID_HEIGHT;
        game_state->cats[i].size = 1;

//        for (int j = 0; j < GRID_WIDTH * GRID_HEIGHT; j++) {
//            game_state->cats[i].position.x = -1;
//            game_state->cats[i].position.y = -1;
//        }
    }

    spawn_mice(game_state);

    game_state->game_over = false;
}

void move_cats(GameState *game_state, Input inputs[NUM_PLAYERS]) {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        Position new_position = {game_state->cats[i].position.x + inputs[i].dx, game_state->cats[i].position.y + inputs[i].dy};

        if (new_position.x < 0 || new_position.x >= GRID_WIDTH ||
            new_position.y < 0 || new_position.y >= GRID_HEIGHT) {
            continue;
        }

        for (int j = 0; j < NUM_MICE; j++) {
            if (game_state->mice[j].alive &&
                new_position.x == game_state->mice[j].position.x &&
                new_position.y == game_state->mice[j].position.y) {
                game_state->cats[i].size++;
                game_state->mice[j].alive = false;
            }
        }

        game_state->cats[i].position = new_position;
    }

    for (int i = 0; i < NUM_MICE; i++) {
        if (game_state->mice[i].alive) {
            Position cat_positions[NUM_PLAYERS];
            for (int j = 0; j < NUM_PLAYERS; j++) {
                cat_positions[j] = game_state->cats[j].position;
            }
            game_state->mice[i].position = flee_from_cats(game_state->mice[i].position, cat_positions, NUM_PLAYERS);
        }
    }
}


uint32_t begin(System system, Renderer renderer) {
    renderer.init();

    srand(time(NULL));
    GameState game_state;

    init_game(&game_state);

    bool quit = false;
    Input inputs[NUM_PLAYERS] = {};

    uint32_t last_move_time = system.get_millis();

    while (!quit) {
        system.handle_input_events(&quit, inputs);

        if (!game_state.game_over) {
            uint32_t current_time = system.get_millis();
            if (current_time - last_move_time > 100) {
                // Update cats positions based on player inputs
                move_cats(&game_state, inputs);
                // Update mice positions
                update_mice_positions(&game_state);
                last_move_time = current_time;
            }
        }

        renderer.clear();

        for (int i = 0; i < NUM_PLAYERS; i++) {
            Position catPos = game_state.cats[i].position;
            int x = catPos.x;
            int y = catPos.y;
            renderer.draw_pixel(x, y, 0x00FF00);
        }

        for (int i = 0; i < NUM_MICE; i++) {
            int x = game_state.mice[i].position.x;
            int y = game_state.mice[i].position.y;
            renderer.draw_pixel(x, y, 0xFF0000);
        }

        renderer.present();

        system.delay(10);
    }

    renderer.cleanup();
    return 0;
}

uint32_t end(void) {
    return 0;
}

Game game_create() {
    Game game;
    game.begin = begin;
    game.end = end;
    return game;
}
