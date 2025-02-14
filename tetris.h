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
#include <optional>
#include <random>
#include <vector>

#include "Scene.h"

class Tetris : public Scene {
 private:
  std::vector<std::vector<int>> grid;
  std::vector<std::vector<int>> currentPiece;
  int nextType;
  int heldPieceType;
  bool canSwap;
  int curR;
  int curC;
  int curType;
  int curRotation;
  bool leftPressed = false;
  bool rightPressed = false;
  bool downPressed = false;
  Uint32 lastUpdate;
  uint32_t leftTimer = 0;
  uint32_t rightTimer = 0;
  uint32_t downTimer = 0;
  std::mt19937 gen;
  uint32_t startTime;
  uint32_t finishTime;
  int linesLeft;
  std::string gameOverText;
  bool gameOver;

  void spawnNewPiece(int spawnType = -1);
  bool isColliding(std::vector<std::vector<int>>& piece,
                   int pieceRow,
                   int pieceCol);
  void addCurrentPiece();
  void clearLines();
  void rotateClockwise();
  void rotateCounterClockwise();
  void dropPiece();
  void progressPieces();
  void moveLeft();
  void moveRight();
  void reset();
  std::vector<std::vector<int>> getWallKickData(int curRotation,
                                                int nextRotation);

 public:
  Tetris(SceneManager& sceneManager);

  void render(SDL_Renderer* renderer) override;

  void handleInput(const SDL_Event& event) override;

  void update() override;
};
