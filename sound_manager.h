#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

inline void finishedPreLoop();

class SoundManager {
 private:
  Mix_Music* preloop;
  Mix_Music* loop;
  Mix_Chunk* rotate;
  Mix_Chunk* drop;
  Mix_Chunk* yay;
  Mix_Chunk* lose;

 public:
  SoundManager() {
    preloop = Mix_LoadMUS("assets/preloop.ogg");
    if (!preloop) {
      throw std::exception();
    }
    loop = Mix_LoadMUS("assets/loop.ogg");
    if (!loop) {
      throw std::exception();
    }
    rotate = Mix_LoadWAV("assets/rotate.wav");
    if (!rotate) {
      throw std::exception();
    }
    drop = Mix_LoadWAV("assets/drop.mp3");
    if (!drop) {
      throw std::exception();
    }
    yay = Mix_LoadWAV("assets/yay.mp3");
    if (!yay) {
      throw std::exception();
    }
    lose = Mix_LoadWAV("assets/gameover.wav");
    if (!lose) {
      throw std::exception();
    }
  }

  static SoundManager& getInstance() {
    static SoundManager soundManager;
    return soundManager;
  }

  void continueMainTheme() { Mix_PlayMusic(loop, -1); }

  void startMainTheme() {
    Mix_VolumeMusic(24);
    Mix_FadeInMusic(preloop, 0, 300);
    Mix_HookMusicFinished(finishedPreLoop);
  }

  void playRotate() { Mix_PlayChannel(-1, rotate, 0); }

  void playDrop() { Mix_PlayChannel(-1, drop, 0); }

  void playYay() {
    Mix_FadeOutMusic(50);
    Mix_HookMusicFinished(NULL);
    Mix_PlayChannel(-1, yay, 0);
  }

  void playLose() {
    Mix_FadeOutMusic(50);
    Mix_HookMusicFinished(NULL);
    Mix_PlayChannel(-1, lose, 0);
  }

  ~SoundManager() {
    Mix_FreeMusic(preloop);
    Mix_FreeMusic(loop);
    Mix_FreeChunk(rotate);
    Mix_FreeChunk(drop);
    Mix_FreeChunk(yay);
    Mix_FreeChunk(lose);
  }
};

inline void finishedPreLoop() {
  SoundManager::getInstance().continueMainTheme();
}
