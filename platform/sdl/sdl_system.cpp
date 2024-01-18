#include <system.h>
#include <renderer.h>
#include <game.h>
#include <SDL.h>
#include <SDL_ttf.h>

// #include <cstdbool.h>
// #include <cstdlib.h>
// #include <ctime.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BLOCK_SIZE = 20;
const int GRID_WIDTH = 32;
const int GRID_HEIGHT = 24;

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

int main() {
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
        printy("delta: %d\n", Time.delta);

        if (Time.delta < game.get_millis_per_frame()) continue;
        Time.time = time;

        renderer_start_frame();
        // encoder_updateAll();
        // chuck_updateAll();

        for (size_t i = 0; i < N_CONTROLLERS; i++)
        {
            Controller *c = &controllers[i];
            c->active = false;

            // if (encoder_isActive(i))
            // {
            //     c->active = true;
            //     c->position = encoders[i].position;
            // }

            // if (chuck_isActive(i))
            // {
            //     c->active = true;
            //     c->x = chucks[i].x;
            //     c->y = chucks[i].y;
            // }

        #ifdef PRINT_CONTROLLER_STATE
            Serial.printf("Controller %d: active: %d, x: %d, y: %d, position: %d\n", i, c->active, c->x, c->y, c->position);
        #endif
        }

        game.loop();
        renderer_end_frame();
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
