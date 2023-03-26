#include <renderer.h>
#include <SDL.h>
#include <SDL_ttf.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BLOCK_SIZE = 20;

static SDL_Window *window = NULL;
static SDL_Renderer *sdl_renderer = NULL;

static void sdl_init() {
    fprintf(stderr, "sdl_init start\n");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return;
    }

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl_renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    fprintf(stderr, "sdl_init complete\n");
}

static void sdl_clear() {
    SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(sdl_renderer);
}

static void sdl_draw_pixel(int x, int y, uint32_t color) {
    SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
    SDL_SetRenderDrawColor(sdl_renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
    SDL_RenderFillRect(sdl_renderer, &rect);
}

static void sdl_draw_rect(int x, int y, int width, int height, uint32_t color) {
    fprintf(stderr, "sdl_draw_rect x and y: %d, %d  width and height: %d, %d\n", x, y, width, height);

    SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, width * BLOCK_SIZE, height * BLOCK_SIZE};
    // SDL_SetRenderDrawColor(sdl_renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
    SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(sdl_renderer, &rect);
}

static void sdl_present() {
    SDL_RenderPresent(sdl_renderer);
}

static void sdl_cleanup() {
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

extern Renderer renderer_create() {
    Renderer renderer;
    renderer.init = sdl_init;
    renderer.clear = sdl_clear;
    renderer.draw_pixel = sdl_draw_pixel;
    renderer.draw_rect = sdl_draw_rect;
    renderer.present = sdl_present;
    renderer.cleanup = sdl_cleanup;
    return renderer;
}
