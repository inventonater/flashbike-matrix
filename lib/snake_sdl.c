// brew install sdl2
// brew install sdl2_ttf

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BLOCK_SIZE = 20;
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
    Position food;
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
    game_state->food = food_position;
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
    Position new_position = { game_state->snake.position.x + dx, game_state->snake.position.y + dy };

    if (new_position.x < 0 || new_position.x >= GRID_WIDTH ||
        new_position.y < 0 || new_position.y >= GRID_HEIGHT) {
        game_state->game_over = true;
        return;
    }

    for (int i = 0; i < game_state->snake.length; i++) {
        if (game_state->snake.body[i].x == new_position.x &&
            game_state->snake.body[i].y == new_position.y) {
            game_state->game_over = true;
            return;
        }
    }

    for (int i = game_state->snake.length - 1; i > 0; i--) {
        game_state->snake.body[i] = game_state->snake.body[i - 1];
    }

    game_state->snake.body[0] = new_position;
    game_state->snake.position = new_position;

    if (new_position.x == game_state->food.x && new_position.y == game_state->food.y) {
        game_state->snake.length++;
        spawn_food(game_state);
    }
}

int main_() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    srand(time(NULL));
    GameState game_state;
   
    init_game(&game_state);

    SDL_Event event;
    bool quit = false;
    int dx = 1, dy = 0;
    Uint32 last_move_time = SDL_GetTicks();

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        dx = 0;
                        dy = -1;
                        break;
                    case SDLK_DOWN:
                        dx = 0;
                        dy = 1;
                        break;
                    case SDLK_LEFT:
                        dx = -1;
                        dy = 0;
                        break;
                    case SDLK_RIGHT:
                        dx = 1;
                        dy = 0;
                        break;
                }
            }
        }

        if (!game_state.game_over) {
            Uint32 current_time = SDL_GetTicks();
            if (current_time - last_move_time > 100) {
                move_snake(&game_state, dx, dy);
                last_move_time = current_time;
            }
        }

        // Clear
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw Rect
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (int i = 0; i < game_state.snake.length; i++) {
            SDL_Rect rect = { game_state.snake.body[i].x * BLOCK_SIZE, game_state.snake.body[i].y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
            SDL_RenderFillRect(renderer, &rect);
        }

        // Draw Rect
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect food_rect = { game_state.food.x * BLOCK_SIZE, game_state.food.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
        SDL_RenderFillRect(renderer, &food_rect);

        // Preset
        SDL_RenderPresent(renderer);

        // Delay
        SDL_Delay(10);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
