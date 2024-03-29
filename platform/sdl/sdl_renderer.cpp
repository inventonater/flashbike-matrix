#include <renderer.h>
#include <SDL.h>
#include <SDL_ttf.h>

const int BLOCK_SIZE = 5;

static SDL_Window *window = (SDL_Window*)NULL;
static SDL_Renderer *sdl_renderer = (SDL_Renderer*)NULL;

void renderer_init(RenderContext context)
{
    fprintf(stderr, "sdl_init start\n");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return;
    }

    int w = context.width * BLOCK_SIZE;
    int h = context.height * BLOCK_SIZE;
    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
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

void renderer_start_frame() {
    fprintf(stderr, "renderer_start_frame\n");

    SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(sdl_renderer);
}

void renderer_end_frame() {
    fprintf(stderr, "renderer_end_frame\n");
    SDL_RenderPresent(sdl_renderer);
}

// set render draw color from color_t
void renderer_set_color(color_t color565) {
    uint8_t r = (color565 >> 11) & 0x1F;
    uint8_t g = (color565 >> 5) & 0x3F;
    uint8_t b = color565 & 0x1F;

    // Scale values to 8-bit
    r = (r * 255) / 31;
    g = (g * 255) / 63;
    b = (b * 255) / 31;

    SDL_SetRenderDrawColor(sdl_renderer, r, g, b, 255);
}

void renderer_write_pixel(int x, int y, color_t color) {
    renderer_set_color(color);
    SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
    SDL_RenderFillRect(sdl_renderer, &rect);
}

void renderer_draw_rect(int x, int y, int width, int height, color_t color) {
    renderer_set_color(color);
    SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, width * BLOCK_SIZE, height * BLOCK_SIZE};
    SDL_RenderFillRect(sdl_renderer, &rect);
}

// unused
void renderer_cleanup() {
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}