#include <system.h>
#include <SDL.h>

uint32_t system_get_millis(void) {
    return SDL_GetTicks();
}

void system_delay(uint32_t ms) {
    SDL_Delay(ms);
}

void system_handle_input_events(bool *quit, int *dx, int *dy) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *quit = true;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_UP) {
                *dx = 0;
                *dy = -1;
            } else if (event.key.keysym.sym == SDLK_DOWN) {
                *dx = 0;
                *dy = 1;
            } else if (event.key.keysym.sym == SDLK_LEFT) {
                *dx = -1;
                *dy = 0;
            } else if (event.key.keysym.sym == SDLK_RIGHT) {
                *dx = 1;
                *dy = 0;
            }
        }
    }
}

extern System system_create() {
    System system;
    system.get_millis = system_get_millis;
    system.delay = system_delay;
    system.handle_input_events = system_handle_input_events;
    return system;
}
