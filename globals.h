#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int DEFAULT_SCREEN_WIDTH = 600;
const int DEFAULT_SCREEN_HEIGHT = 800;

namespace Globals {
inline int screenWidth = DEFAULT_SCREEN_WIDTH;
inline int screenHeight = DEFAULT_SCREEN_HEIGHT;
inline TTF_Font* openSans = nullptr;
inline SDL_Renderer* renderer = nullptr;
}  // namespace Globals
