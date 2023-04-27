#include <system.h>
#include <renderer.h>
#include <game.h>
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

int main() {
    const Game game = game_create();
    game.begin();
}