#include "game.h"
#include "system.h"
#include "renderer.h"
#include "util.h"

#define PLAYER_COLOR 0xFFFFFF
#define FINISH_LINE_COLOR 0x00FF00
#define PLAYER_START_Y 0
#define FINISH_LINE_Y 15
#define PLAYER_SIZE 1
#define INPUT_THRESHOLD 2

static System sys;
static Renderer renderer;

static void draw_players(int p1_x, int p2_x) {
    renderer.draw_rect(p1_x, PLAYER_START_Y, PLAYER_SIZE, PLAYER_SIZE, PLAYER_COLOR);
    renderer.draw_rect(p2_x, PLAYER_START_Y, PLAYER_SIZE, PLAYER_SIZE, PLAYER_COLOR);
}

static void draw_finish_line() {
    for (int x = 0; x < renderer.width; x++) {
        renderer.draw_pixel(x, FINISH_LINE_Y, FINISH_LINE_COLOR);
    }
}

static uint32_t game_begin(System system, Renderer rend) {
    sys = system;
    renderer = rend;

    int p1_x = renderer.width / 4;
    int p2_x = (3 * renderer.width) / 4;
    int p1_dx = 0, p2_dx = 0;
    int p1_dy = 0, p2_dy = 0;
    bool quit = false;

    while (!quit && p1_dy < FINISH_LINE_Y && p2_dy < FINISH_LINE_Y) {
        sys.handle_input_events(&quit, &p1_dx, &p2_dx);

        if (abs(p1_dx) >= INPUT_THRESHOLD) {
            p1_x += p1_dx / INPUT_THRESHOLD;
            if (p1_x < 0) p1_x = 0;
            if (p1_x + PLAYER_SIZE > renderer.width) p1_x = renderer.width - PLAYER_SIZE;
        }

        if (abs(p2_dx) >= INPUT_THRESHOLD) {
            p2_x += p2_dx / INPUT_THRESHOLD;
            if (p2_x < 0) p2_x = 0;
            if (p2_x + PLAYER_SIZE > renderer.width) p2_x = renderer.width - PLAYER_SIZE;
        }

        p1_dy += p1_dx;
        p2_dy += p2_dx;

        renderer.clear();
        draw_finish_line();
        draw_players(p1_x, p2_x);
        renderer.present();

        sys.delay(secToMillis(1) / 30);
    }

    return (p1_dy >= FINISH_LINE_Y) ? 1 : 2;
}

static uint32_t game_end() {
    return 0;
}

Game game_create() {
    Game game;
    game.begin = game_begin;
    game.end = game_end;
    return game;
}