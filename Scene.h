#include <SDL2/SDL.h>

#include <SDL2/SDL_events.h>
#include <memory>

class Scene;

class SceneManager {
public:
  std::shared_ptr<Scene> curScene;
  void change(std::shared_ptr<Scene> newScene) { curScene = newScene; }
};

class Scene {
private:
  SceneManager &sceneManager;

public:
  virtual void handleInput(const SDL_Event &event) = 0;
  virtual void update() = 0;
  virtual void render(SDL_Renderer *renderer) = 0;
  virtual bool shouldQuit() = 0;
  Scene(SceneManager &manager) : sceneManager(manager) {};
};
