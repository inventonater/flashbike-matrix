#include <renderer.h>
#include <SDL.h>
#include <SDL_ttf.h>

const int BLOCK_SIZE = 3;

static SDL_Window *window = (SDL_Window*)NULL;
static SDL_Renderer *sdl_renderer = (SDL_Renderer*)NULL;

void renderer_init(uint16_t width, uint16_t height) 
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

    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * BLOCK_SIZE, height * BLOCK_SIZE, SDL_WINDOW_SHOWN);
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

void renderer_write_pixel(int x, int y, color_t color) {
    SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
    SDL_SetRenderDrawColor(sdl_renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
    SDL_RenderFillRect(sdl_renderer, &rect);
}

void renderer_draw_rect(int x, int y, int width, int height, color_t color) {
    fprintf(stderr, "sdl_draw_rect x and y: %d, %d  width and height: %d, %d\n", x, y, width, height);

    SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, width * BLOCK_SIZE, height * BLOCK_SIZE};
    // SDL_SetRenderDrawColor(sdl_renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
    SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(sdl_renderer, &rect);
}

// unused
void renderer_cleanup() {
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}