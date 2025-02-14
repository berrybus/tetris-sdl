#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "font_manager.h"
#include "tetris.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <sys/types.h>
#include <ctime>
#include <iostream>
#include <memory>

const int DEFAULT_SCREEN_WIDTH = 1024;
const int DEFAULT_SCREEN_HEIGHT = 768;

SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* openSans;

void close() {
  TTF_CloseFont(openSans);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "could not init!" << std::endl;
    return 1;
  }

  window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, DEFAULT_SCREEN_WIDTH,
                            DEFAULT_SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    std::cout << "could not create window!" << std::endl;
    return 1;
  }
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == NULL) {
    std::cout << "could not create renderer!" << std::endl;
    return 1;
  }

  if (TTF_Init() == -1) {
    std::cout << "could not init fonts!" << std::endl;
    return 1;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  FontManager::getInstance().initialize(renderer);
  SceneManager sceneManager = SceneManager();
  sceneManager.change(std::make_shared<Tetris>(sceneManager));
  SDL_Event event;
  while (true) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        close();
        return 0;
      }

      sceneManager.curScene->handleInput(event);
    }
    sceneManager.curScene->update();
    sceneManager.curScene->render(renderer);
    SDL_RenderPresent(renderer);
  }
  close();
  return 0;
}
