#include <system.h>
#include <renderer.h>
#include <game.h>
#include <SDL.h>
#include <SDL_ttf.h>

// #include <cstdbool.h>
// #include <cstdlib.h>
// #include <ctime.h>


#define TICK_RATE_MILLIS 20
sys_time_t Time;

uint32_t system_get_millis(void) {
    return SDL_GetTicks();
}

void system_delay(uint32_t ms) {
    SDL_Delay(ms);
}

void system_handle_input_events(bool *quit, int *dx, int *dy) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) *quit = true;
        if (event.type != SDL_KEYDOWN) return;
        SDL_Keycode sym = event.key.keysym.sym;
        if (sym == SDLK_UP) { *dx = 0; *dy = -1; }
        if (sym == SDLK_UP) { *dx = 0; *dy = -1; }
        if (sym == SDLK_DOWN) { *dx = 0; *dy = 1; }
        if (sym == SDLK_LEFT) { *dx = -1; *dy = 0; }
        if (sym == SDLK_RIGHT) { *dx = 1; *dy = 0; }
    }
}

#define N_CONTROLLERS 4
Controller controllers[N_CONTROLLERS] = {};
const Controller *system_get_controller(uint8_t index)
{
  if (index < N_CONTROLLERS) return &controllers[index];
  return NULL;
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

    renderer_init();
    const Game game = game_create();
    game.begin();

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

        uint32_t time = system_get_millis();
        Time.delta = time - Time.time;
        if (Time.delta < TICK_RATE_MILLIS) return;
        Time.time = time;

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
