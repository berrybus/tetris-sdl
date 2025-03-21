#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "font_manager.h"

const std::vector<std::tuple<std::string, int>> fontLocations = {
    {"assets/open_sans.ttf", 24},
    {"assets/roboto.ttf", 36}};

void FontManager::initialize(SDL_Renderer* p_renderer) {
  renderer = p_renderer;
  for (auto& pair : fontLocations) {
    auto& location = std::get<0>(pair);
    auto& size = std::get<1>(pair);
    TTF_Font* font = TTF_OpenFont(location.c_str(), size);
    if (!font) {
      throw std::exception();
    }
    SDL_Color white = {255, 255, 255, 255};
    std::unordered_map<char, SDL_Texture*> font_map;
    for (uint8_t c = 0; c <= 127; c++) {
      char str[2] = {(char)c, '\0'};
      SDL_Surface* surface = TTF_RenderText_Blended(font, str, white);
      SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
      font_map[c] = texture;
      SDL_FreeSurface(surface);
    }
    font_to_cache.push_back({font, font_map});
  }
};

std::pair<int, int> FontManager::getTextSize(std::string text, int font) {
  int x = 0;
  int y = 0;
  for (char c : text) {
    SDL_Texture* texture = std::get<1>(font_to_cache.at(font))[c];
    int w;
    int h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    if (c == '\n') {
      y += h;
    } else {
      x += w;
      if (y == 0) {
        y += h;
      }
    }
  }

  return {x, y};
}

void FontManager::renderText(int x, int y, std::string text, int font) {
  int startingX = x;
  int startingY = y;
  for (char c : text) {
    SDL_Texture* texture = std::get<1>(font_to_cache.at(font))[c];
    int w;
    int h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    if (c == '\n') {
      startingY += h;
      startingX = x;
    } else {
      SDL_Rect rect = {startingX, startingY, w, h};
      SDL_RenderCopy(renderer, texture, NULL, &rect);
      startingX += w;
    }
  }
};
