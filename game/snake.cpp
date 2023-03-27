#include "system.h"
#include "renderer.h"
#include "game.h"

#include <stdlib.h>
#include <time.h>

const int GRID_WIDTH = 32;
const int GRID_HEIGHT = 24;

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position position;
    int length;
    Position body[GRID_WIDTH * GRID_HEIGHT];
} Snake;

typedef struct {
    Snake snake;
    Position food_position;
    bool game_over;
} GameState;

bool is_food_valid(const GameState *game_state, Position food_position) {
    for (int i = 0; i < game_state->snake.length; i++) {
        if (game_state->snake.body[i].x == food_position.x &&
            game_state->snake.body[i].y == food_position.y) {
            return false;
        }
    }
    return true;
}

void spawn_food(GameState *game_state) {
    Position food_position;
    do {
        food_position.x = rand() % GRID_WIDTH;
        food_position.y = rand() % GRID_HEIGHT;
    } while (!is_food_valid(game_state, food_position));
    game_state->food_position = food_position;
}

void init_game(GameState *game_state) {
    game_state->snake.position.x = GRID_WIDTH / 2;
    game_state->snake.position.y = GRID_HEIGHT / 2;
    game_state->snake.length = 1;

    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        game_state->snake.body[i].x = -1;
        game_state->snake.body[i].y = -1;
    }

    spawn_food(game_state);

    game_state->game_over = false;
}

void move_snake(GameState *game_state, int dx, int dy) {
    Position new_position = {game_state->snake.position.x + dx, game_state->snake.position.y + dy};

    if (new_position.x < 0 || new_position.x >= GRID_WIDTH ||
        new_position.y < 0 || new_position.y >= GRID_HEIGHT) {
        game_state->game_over = true;
        return;
    }

    for (int i = 0; i < game_state->snake.length; i++) {
        Position p = game_state->snake.body[i];
        if (p.x == new_position.x && p.y == new_position.y) {
            game_state->game_over = true;
            return;
        }
    }

    for (int i = game_state->snake.length - 1; i > 0; i--) {
        game_state->snake.body[i] = game_state->snake.body[i - 1];
    }

    game_state->snake.body[0] = new_position;
    game_state->snake.position = new_position;

    if (new_position.x == game_state->food_position.x && new_position.y == game_state->food_position.y) {
        game_state->snake.length++;
        spawn_food(game_state);
    }
}

uint32_t begin(System system, Renderer renderer) {
    renderer.init();

    srand(time(NULL));
    GameState game_state;

    init_game(&game_state);

    bool quit = false;
    int dx = 1, dy = 0;

    uint32_t last_move_time = system.get_millis();

    while (!quit) {
        system.handle_input_events(&quit, &dx, &dy);

        if (!game_state.game_over) {
            uint32_t current_time = system.get_millis();
            if (current_time - last_move_time > 100) {
                move_snake(&game_state, dx, dy);
                last_move_time = current_time;
            }
        }

        renderer.clear();

        for (int i = 0; i < game_state.snake.length; i++) {
            Position bodyPos = game_state.snake.body[i];
            int x = bodyPos.x;
            int y = bodyPos.y;
            renderer.draw_pixel(x, y, 0x00FF00);
        }

        int x = game_state.food_position.x;
        int y = game_state.food_position.y;
        renderer.draw_pixel(x, y, 0xFF0000);

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
