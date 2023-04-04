#include "game.h"
#include "system.h"
#include "renderer.h"
#include "util.h"

#define PLAYER_COLOR 0xFFFFFF
#define CIVILIAN_COLOR 0x00FF00
#define ZOMBIE_COLOR 0xFF0000
#define PLAYER_SIZE 1
#define CIVILIAN_SIZE 1
#define ZOMBIE_SIZE 1
#define PLAYER_SPEED 1
#define ZOMBIE_SPEED 1
#define CIVILIAN_SPEED 1
#define PLAYER_SHOTS 5
#define RELOAD_TIME 3000
#define DETECTION_RADIUS 5
#define COLLISION_DISTANCE 1
#define MAX_ZOMBIES 100
#define INITIAL_ZOMBIES 5


typedef struct
{
    int x;
    int y;
} Entity;

static System sys;
static Renderer renderer;
static Entity players[4];
static Entity civilians[10];
Entity zombies[MAX_ZOMBIES];

static void draw_entity(Entity entity, uint32_t color)
{
    renderer.draw_rect(entity.x, entity.y, PLAYER_SIZE, PLAYER_SIZE, color);
}

static void draw_entities(Entity entities[], int count, uint32_t color)
{
    for (int i = 0; i < count; i++)
    {
        draw_entity(entities[i], color);
    }
}

static bool is_collision(Entity a, Entity b)
{
    return a.x == b.x && a.y == b.y;
}

static void initialize_players() {
    players[0] = (Entity){.x = renderer.width / 8, .y = renderer.height / 2};
    players[1] = (Entity){.x = (3 * renderer.width) / 8, .y = renderer.height / 2};
    players[2] = (Entity){.x = (5 * renderer.width) / 8, .y = renderer.height / 2};
    players[3] = (Entity){.x = (7 * renderer.width) / 8, .y = renderer.height / 2};
}

static void initialize_civilians() {
    // Randomly initialize civilians within the LED matrix.
    srand(time(NULL));
    for (int i = 0; i < sizeof(civilians) / sizeof(civilians[0]); i++) {
        civilians[i].x = rand() % renderer.width;
        civilians[i].y = rand() % renderer.height;
    }
}

static void initialize_zombies() {
    for (int i = 0; i < INITIAL_ZOMBIES; i++) {
        zombies[i].x = rand() % MATRIX_WIDTH;
        zombies[i].y = rand() % MATRIX_HEIGHT;
    }
    // Set the remaining zombies' positions to (-1, -1)
    for (int i = INITIAL_ZOMBIES; i < MAX_ZOMBIES; i++) {
        zombies[i].x = -1;
        zombies[i].y = -1;
    }
}

static bool is_in_line_of_sight(Entity player, Entity target) {
    return player.y == target.y;
}

static void shoot_zombie(int player_index) {
    for (int i = 0; i < sizeof(zombies) / sizeof(zombies[0]); i++) {
        if (is_in_line_of_sight(players[player_index], zombies[i])) {
            // Remove the zombie by moving it off the screen.
            zombies[i].x = -1;
            zombies[i].y = -1;

            // You can also implement a scoring system or other game mechanics here.
            break;
        }
    }
}

static Entity find_closest(Entity entities[], int count, Entity target) {
    Entity closest_entity = entities[0];
    int min_distance = abs(target.x - closest_entity.x) + abs(target.y - closest_entity.y);

    for (int i = 1; i < count; i++) {
        int distance = abs(target.x - entities[i].x) + abs(target.y - entities[i].y);
        if (distance < min_distance) {
            min_distance = distance;
            closest_entity = entities[i];
        }
    }

    return closest_entity;
}

static void move_towards(Entity *mover, Entity target, int speed) {
    if (mover->x < target.x) mover->x += speed;
    else if (mover->x > target.x) mover->x -= speed;

    if (mover->y < target.y) mover->y += speed;
    else if (mover->y > target.y) mover->y -= speed;
}

static void move_away(Entity *mover, Entity target, int speed) {
    if (mover->x < target.x) mover->x -= speed;
    else if (mover->x > target.x) mover->x += speed;

    if (mover->y < target.y) mover->y -= speed;
    else if (mover->y > target.y) mover->y += speed;
}

static void update_zombie_positions() {
    for (int i = 0; i < sizeof(zombies) / sizeof(zombies[0]); i++) {
        Entity closest_civilian = find_closest(civilians, sizeof(civilians) / sizeof(civilians[0]), zombies[i]);
        int distance = abs(zombies[i].x - closest_civilian.x) + abs(zombies[i].y - closest_civilian.y);

        if (distance <= DETECTION_RADIUS) {
            move_towards(&zombies[i], closest_civilian, ZOMBIE_SPEED);
        }
    }
}

static void update_civilian_positions() {
    for (int i = 0; i < sizeof(civilians) / sizeof(civilians[0]); i++) {
        Entity closest_zombie = find_closest(zombies, sizeof(zombies) / sizeof(zombies[0]), civilians[i]);
        int distance = abs(civilians[i].x - closest_zombie.x) + abs(civilians[i].y - closest_zombie.y);

        if (distance <= DETECTION_RADIUS) {
            move_away(&civilians[i], closest_zombie, CIVILIAN_SPEED);
        }
    }
}

static bool is_touching(Entity a, Entity b) {
    int distance = abs(a.x - b.x) + abs(a.y - b.y);
    return distance <= COLLISION_DISTANCE;
}

static void turn_civilian_into_zombie() {
    for (int i = 0; i < sizeof(civilians) / sizeof(civilians[0]); i++) {
        for (int j = 0; j < sizeof(zombies) / sizeof(zombies[0]); j++) {
            if (is_touching(civilians[i], zombies[j])) {
                // Turn the civilian into a zombie by moving them off the screen
                civilians[i].x = -1;
                civilians[i].y = -1;
                remaining_civilians--;

                // Find an available slot in the zombies array (zombie moved off the screen)
                int available_zombie_slot = -1;
                for (int k = 0; k < sizeof(zombies) / sizeof(zombies[0]); k++) {
                    if (zombies[k].x == -1 && zombies[k].y == -1) {
                        available_zombie_slot = k;
                        break;
                    }
                }

                // If an available slot is found, create a new zombie at the same position as the converted civilian
                if (available_zombie_slot != -1) {
                    zombies[available_zombie_slot] = (Entity){.x = zombies[j].x, .y = zombies[j].y};
                }

                break;
            }
        }
    }
}


static uint32_t game_begin(System system, Renderer rend)
{
    sys = system;
    renderer = rend;


    initialize_players();
    initialize_civilians();
    initialize_zombies();

    int remaining_civilians = sizeof(civilians) / sizeof(civilians[0]);
    int player_shots[4] = {PLAYER_SHOTS, PLAYER_SHOTS, PLAYER_SHOTS, PLAYER_SHOTS};
    uint32_t player_reload_end[4] = {0, 0, 0, 0};
    bool quit = false;

    while (!quit && remaining_civilians > 0)
    {
        int dx[4] = {0, 0, 0, 0};
        int shoot[4] = {0, 0, 0, 0};
        int reload[4] = {0, 0, 0, 0};

        sys.handle_input_events(&quit, dx, shoot, reload);

        for (int i = 0; i < 4; i++)
        {
            if (sys.get_millis() > player_reload_end[i]) {
                if (shoot[i]) {
                    if (player_shots[i] > 0) {
                        shoot_zombie(i);
                        player_shots[i]--;
                    }
                } else if (reload[i]) {
                    player_reload_end[i] = sys.get_millis() + RELOAD_TIME;
                }
            }
        }

        // Update zombie and civilian positions.
        update_zombie_positions();
        update_civilian_positions();

        // Check for collisions between zombies and civilians.
        for (int i = 0; i < sizeof(zombies) / sizeof(zombies[0]); i++)
        {
            for (int j = 0; j < sizeof(civilians) / sizeof(civilians[0]); j++)
            {
                if (is_collision(zombies[i], civilians[j]))
                {
                    // Turn civilian into a zombie.
                    turn_civilian_into_zombie();
                    remaining_civilians--;
                }
            }
        }

        renderer.clear();
        draw_entities(players, 4, PLAYER_COLOR);
        draw_entities(civilians, remaining_civilians, CIVILIAN_COLOR);
        draw_entities(zombies, sizeof(zombies) / sizeof(zombies[0]), ZOMBIE_COLOR);
        renderer.present();

        sys.delay(secToMillis(1) / 30);
    }

    return (remaining_civilians == 0) ? 0 : 1;
}

static uint32_t game_end()
{
    return 0;
}

Game game_create()
{
    Game game;
    game.begin = game_begin;
    game.end = game_end;
    return game;
}