#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <sys/types.h>

#include <cstdint>
#include <ctime>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include "Scene.h"
#include "tetris.h"

class Menu : public Scene {
 public:
  Menu(SceneManager& sceneManager) : Scene(sceneManager) {}

  void render(SDL_Renderer* renderer) override {
    FontManager::getInstance().renderText(80, 60, "40L TETRIS", 1);
    FontManager::getInstance().renderText(80, 110, "Press any key to start", 0);
  }

  void handleInput(const SDL_Event& event) override {
    if (event.type == SDL_KEYDOWN) {
      sceneManager.change(std::make_shared<Tetris>(sceneManager));
    }
  }

  void update() override {};
};
