#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class FontManager {
 private:
  std::vector<std::tuple<TTF_Font*, std::unordered_map<char, SDL_Texture*>>>
      font_to_cache;

  SDL_Renderer* renderer;

 public:
  FontManager() = default;

  static FontManager& getInstance() {
    static FontManager fontManager;
    return fontManager;
  }

  ~FontManager() {
    for (auto& font : font_to_cache) {
      for (auto& texture : std::get<1>(font)) {
        SDL_DestroyTexture(texture.second);
      }
      TTF_CloseFont(std::get<0>(font));
    }
  };

  void initialize(SDL_Renderer* p_renderer);

  std::pair<int, int> getTextSize(std::string text, int font);

  void renderText(int x, int y, std::string text, int font);
};
