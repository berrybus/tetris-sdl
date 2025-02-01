#include "Tetris.h"
#include <SDL2/SDL.h>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <ctime>
#include <iostream>
#include <memory>
#include <sys/types.h>

#define SCREEN_WIDTH 300
#define SCREEN_HEIGHT 600

int main() {
  SDL_Window *window =
      SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  SceneManager sceneManager = SceneManager();
  sceneManager.change(std::make_shared<Tetris>(sceneManager));
  SDL_Event event;
  while (!sceneManager.curScene->shouldQuit()) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        return 0;
      }

      sceneManager.curScene->handleInput(event);
    }
    sceneManager.curScene->update();
    sceneManager.curScene->render(renderer);
    SDL_RenderPresent(renderer);
  }
  return 0;
}
