#include <system.h>
#include <renderer.h>
#include <game.h>
#include <SDL.h>
#include <SDL_ttf.h>

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
    const Game game = game_create();
    game.begin();

    uint16_t width = game.get_grid_width();
    uint16_t height = game.get_grid_height();
    renderer_init(width, height);

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

    renderer_cleanup();
    return 0;
}
