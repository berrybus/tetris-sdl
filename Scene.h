#pragma once

#include <SDL2/SDL.h>

#include <SDL2/SDL_events.h>
#include <memory>
#include "font_manager.h"

class Scene;

class SceneManager {
 public:
  std::shared_ptr<Scene> curScene;
  void change(std::shared_ptr<Scene> newScene) { curScene = newScene; }
  SceneManager() = default;
};

class Scene {
 public:
  SceneManager& sceneManager;
  virtual void handleInput(const SDL_Event& event) = 0;
  virtual void update() = 0;
  virtual void render(SDL_Renderer* renderer) = 0;
  Scene(SceneManager& manager) : sceneManager(manager) {};
};
