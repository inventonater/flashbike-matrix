#include <Arduino_GFX.h>
#include <SDL2/SDL.h>
#include <iostream>

class SDL_GFX : public Arduino_GFX
{
public:
  SDL_GFX(int16_t w, int16_t h) : Arduino_GFX(w, h),
                                  window(nullptr),
                                  renderer(nullptr),
                                  lastKeyPressed(SDLK_UNKNOWN)
  {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
      // Consider setting an error flag here
    }
    else
    {
      window = SDL_CreateWindow("SDL GFX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
      if (window == nullptr)
      {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        // Handle window creation failure
      }
      else
      {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr)
        {
          std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
          // Handle renderer creation failure
        }
      }
    }
  }

  void handleEvents()
  {
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
      if (e.type == SDL_QUIT)
      {
        // Handle quit event
      }
      else if (e.type == SDL_KEYDOWN)
      {
        lastKeyPressed = e.key.keysym.sym; // Store the last pressed key

        switch (lastKeyPressed)
        {
        case SDLK_UP:
          std::cout << "Up arrow key pressed" << std::endl;
          break;
        case SDLK_DOWN:
          std::cout << "Down arrow key pressed" << std::endl;
          break;
        case SDLK_LEFT:
          std::cout << "Left arrow key pressed" << std::endl;
          break;
        case SDLK_RIGHT:
          std::cout << "Right arrow key pressed" << std::endl;
          break;
        }
      }
    }
  }

  // Method to get the last pressed key
  SDL_Keycode getLastKeyPressed()
  {
    return lastKeyPressed;
  }

  void begin(void) override
  {
    // implement me
  }

  void drawPixel(int16_t x, int16_t y, uint16_t color) override
  {
    SDL_SetRenderDrawColor(renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
    SDL_RenderDrawPoint(renderer, x, y);
  }

  void endWrite(void) override
  {
    SDL_RenderPresent(renderer);
  }

  ~SDL_GFX()
  {
    if (renderer)
    {
      SDL_DestroyRenderer(renderer);
    }
    if (window)
    {
      SDL_DestroyWindow(window);
    }
    SDL_Quit();
  }

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Keycode lastKeyPressed;
};