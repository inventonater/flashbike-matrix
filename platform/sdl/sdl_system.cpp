#include <system.h>
#include <renderer.h>
#include <game.h>
#include <SDL.h>
#include <SDL_ttf.h>

sys_time_t Time;

struct KeyboardInput {
    int8_t x, y;
    bool quit;
};

KeyboardInput poll_keyboard() {
    KeyboardInput input = {};
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        input.quit = event.type == SDL_QUIT;
        if (event.type != SDL_KEYDOWN) continue;

        SDL_Keycode sym = event.key.keysym.sym;
        if (sym == SDLK_ESCAPE) { input.quit = true; }
        if (sym == SDLK_UP) { input.x = 0; input.y = -1; }
        if (sym == SDLK_DOWN) { input.x = 0; input.y = 1; }
        if (sym == SDLK_LEFT) { input.x = 1; input.y = 0; }
        if (sym == SDLK_RIGHT) { input.x = -1; input.y = 0; }
    }
    return input;
}

uint32_t system_get_millis(void) {
    return SDL_GetTicks();
}

void system_delay(uint32_t ms) {
    SDL_Delay(ms);
}

#define N_CONTROLLERS 4
Controller controllers[N_CONTROLLERS] = {};

const Controller *system_get_controller(uint8_t index)
{
  if (index < N_CONTROLLERS) return &controllers[index];
  return NULL;
}

Game game = {};
KeyboardInput keyboardInput;

void frame(){
    uint32_t time = system_get_millis();
    Time.delta = time - Time.time;
    if (Time.delta < game.get_millis_per_frame()) return;
    Time.time = time;

    const KeyboardInput &input = poll_keyboard();
    if(input.x != 0 || input.y != 0 || input.quit) keyboardInput = input;

    printy("keyboardInput.x: %d\n", keyboardInput.x);
    printy("keyboardInput.y: %d\n", keyboardInput.y);
    printy("keyboardInput.quit: %d\n", keyboardInput.quit);

    renderer_start_frame();

    for (size_t i = 0; i < N_CONTROLLERS; i++)
    {
        Controller *c = &controllers[i];
        c->active = false;
    }

    Controller *c = &controllers[0];
    c->active = true;
    c->x = keyboardInput.x;
    c->y = keyboardInput.y;

    game.loop();
    renderer_end_frame();
}

int main() {
    game = game_create();
    game.begin();

    renderer_init(game.get_render_context());

    while (true) {
        frame();
        if (keyboardInput.quit) break;
    }

    renderer_cleanup();
    return 0;
}

