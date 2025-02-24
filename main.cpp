#include <SDL2/SDL.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "font_manager.h"
#include "menu.h"

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
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  Mix_Quit();
  SDL_Quit();
}

int main() {
  // There's a bug with the key events repeat handling in Wayland
  setenv("SDL_VIDEODRIVER", "x11", 1);
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cout << "could not init!" << std::endl;
    return 1;
  }

  window = SDL_CreateWindow("40L TETRIS", SDL_WINDOWPOS_CENTERED,
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

  Mix_Init(MIX_INIT_OGG);

  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
                    MIX_DEFAULT_CHANNELS, 2048) < 0) {
    std::cout << "SDL_mixer could not initialize! SDL_mixer Error: "
              << Mix_GetError() << std::endl;
    return 1;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  FontManager::getInstance().initialize(renderer);
  SceneManager sceneManager = SceneManager();
  sceneManager.change(std::make_shared<Menu>(sceneManager));
  SDL_Event event;

  const Uint8* keyState = SDL_GetKeyboardState(NULL);

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
