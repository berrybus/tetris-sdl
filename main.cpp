#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "globals.h"
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

SDL_Window* window;

void close() {
  SDL_DestroyRenderer(Globals::renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "could not init!" << std::endl;
    return -1;
  }

  window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, Globals::screenWidth,
                            Globals::screenHeight, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    std::cout << "could not create window!" << std::endl;
    return -1;
  }
  Globals::renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (Globals::renderer == NULL) {
    std::cout << "could not create renderer!" << std::endl;
    return -1;
  }

  if (TTF_Init() == -1) {
    std::cout << "could not init fonts!" << std::endl;
    return -1;
  }

  Globals::openSans = TTF_OpenFont("open_sans.ttf", 16);

  if (!Globals::openSans) {
    std::cout << "could not init open sans!" << std::endl;
    return -1;
  }

  SceneManager sceneManager = SceneManager();
  sceneManager.change(std::make_shared<Tetris>(sceneManager));
  SDL_Event event;
  while (!sceneManager.curScene->shouldQuit()) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        close();
        return 0;
      }

      sceneManager.curScene->handleInput(event);
    }
    sceneManager.curScene->update();
    sceneManager.curScene->render();
    SDL_RenderPresent(Globals::renderer);
  }
  close();
  return 0;
}
