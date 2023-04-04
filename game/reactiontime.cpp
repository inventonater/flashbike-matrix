#include "game.h"
#include "system.h"
#include "renderer.h"
#include "util.h"

#define PLAYER_COLOR 0xFFFFFF
#define COUNTDOWN_COLOR 0xFFFF00
#define PLAYER_SIZE 1
#define COUNTDOWN_TIME 3
#define INPUT_WAIT_TIME 500

typedef struct {
    int x;
    int y;
} Player;

static System sys;
static Renderer renderer;

static void draw_players(Player players[]) {
    for (int i = 0; i < 4; i++) {
        renderer.draw_rect(players[i].x, players[i].y, PLAYER_SIZE, PLAYER_SIZE, PLAYER_COLOR);
    }
}

static void draw_countdown(int seconds_remaining) {
    for (int y = 0; y < renderer.height; y++) {
        for (int x = 0; x < renderer.width; x++) {
            renderer.draw_pixel(x, y, (seconds_remaining > 0) ? COUNTDOWN_COLOR : 0x000000);
        }
    }
}

static uint32_t game_begin(System system, Renderer rend) {
    sys = system;
    renderer = rend;

    Player players[] = {
        {.x = renderer.width / 8, .y = renderer.height / 2},
        {.x = (3 * renderer.width) / 8, .y = renderer.height / 2},
        {.x = (5 * renderer.width) / 8, .y = renderer.height / 2},
        {.x = (7 * renderer.width) / 8, .y = renderer.height / 2},
    };

    uint32_t start_time = sys.get_millis();
    uint32_t countdown_end = start_time + secToMillis(COUNTDOWN_TIME);
    uint32_t reaction_times[4] = {0, 0, 0, 0};
    uint32_t min_reaction_time = UINT32_MAX;
    int winner = -1;

    while (sys.get_millis() < countdown_end) {
        int seconds_remaining = (countdown_end - sys.get_millis()) / 1000;
        draw_countdown(seconds_remaining);
        renderer.present();
        sys.delay(secToMillis(1) / 30);
    }

    sys.delay(INPUT_WAIT_TIME);

    for (int i = 0; i < 4; i++) {
        uint32_t button_press_time;
        bool button_pressed = sys.wait_for_button_press(i, &button_press_time, INPUT_WAIT_TIME);

        if (button_pressed) {
            reaction_times[i] = button_press_time - countdown_end;

            if (reaction_times[i] < min_reaction_time) {
                min_reaction_time = reaction_times[i];
                winner = i;
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        renderer.clear();
        draw_players(players);
        renderer.present();

        sys.delay(secToMillis(1) / 2);

        renderer.clear();
        renderer.present();

        sys.delay(secToMillis(1) / 2);
    }

    return winner + 1;
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
