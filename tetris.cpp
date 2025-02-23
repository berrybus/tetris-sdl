#include "tetris.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <sys/types.h>
#include <cstdint>
#include <format>
#include "font_manager.h"
#include "rng.h"
#include "sound_manager.h"

const int BLOCK_SIZE = 30;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;
const int GRID_OFFSET_X = 200;
const int GRID_OFFSET_Y = 80;
const int LINES_LEFT = 40;

const Uint32 UPDATE_DELAY = 1000;
const Uint32 LAST_ROW_UPDATE_DELAY = 1500;

const uint32_t DAS_DELAY = 133;
const uint32_t DAS_REPEAT = 10;

const char* INSTRUCTIONS =
    "Arrow keys - move\nUp/Z - rotate\nC - hold\nR - restart";

const std::vector<std::vector<std::vector<int>>> BLOCKS = {
    // I
    {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    // O
    {{1, 1}, {1, 1}},
    // T
    {{0, 1, 0}, {1, 1, 1}, {0, 0, 0}},
    // L
    {{0, 0, 1}, {1, 1, 1}, {0, 0, 0}},
    // J
    {{1, 0, 0}, {1, 1, 1}, {0, 0, 0}},
    // S
    {{0, 1, 1}, {1, 1, 0}, {0, 0, 0}},
    // Z
    {{1, 1, 0}, {0, 1, 1}, {0, 0, 0}}};

const SDL_Color COLORS[] = {{0, 255, 255, 255}, {255, 255, 0, 255},
                            {128, 0, 128, 255}, {255, 127, 0, 255},
                            {0, 0, 255, 255},   {0, 255, 0, 255},
                            {255, 0, 0, 255}};

const std::vector<std::vector<std::vector<int>>> WALL_KICK_I = {
    // 0 -> 1, 3 -> 2
    {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},
    // 1 -> 0, 2 -> 3
    {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},
    // 1 -> 2, 0 -> 3
    {{0, 0}, {-1, 0}, {-2, 0}, {-1, 2}, {2, -1}},
    // 2 -> 1, 3 -> 0
    {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},
};

const std::vector<std::vector<std::vector<int>>> WALL_KICK_NONE_I = {
    // 0 -> 1, 2 -> 1
    {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
    // 1 -> 0, 1 -> 2
    {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
    // 2 -> 3, 0 -> 3
    {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
    // 3 -> 2, 3 -> 0
    {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},
};

int getRandomType() {
  static std::uniform_int_distribution<int> dist(0, BLOCKS.size() - 1);
  return dist(RNG::getInstance().gen);
}

std::string formatMilliseconds(uint32_t ms) {
  int minutes = ms / 60000;
  int seconds = (ms % 60000) / 1000;
  // Only want to display 2 ms digits
  int milliseconds = (ms % 1000) / 10;

  return std::format("Time: {:02}:{:02}.{:02}", minutes, seconds, milliseconds);
}

Tetris::Tetris(SceneManager& sceneManager)
    : Scene(sceneManager),
      grid(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, -1)),
      gameOver(false),
      lastUpdate(SDL_GetTicks()),
      heldPieceType(-1),
      nextType(-1),
      canSwap(true) {
  SDL_Color textColor = {255, 255, 255, 255};
  reset();
  spawnNewPiece();
}

void Tetris::spawnNewPiece(int spawnType) {
  if (spawnType == -1) {
    curType = nextType;
    nextType = getRandomType();
    canSwap = true;
  } else {
    curType = spawnType;
  }

  currentPiece = BLOCKS[curType];
  curC = GRID_WIDTH / 2 - currentPiece[0].size() / 2;
  curR = 0;
  curRotation = 0;
  if (isColliding(currentPiece, curR, curC)) {
    gameOver = true;
    finishTime = SDL_GetTicks();
    gameOverText = "GAME OVER - Press R to restart";
    SoundManager::getInstance().playLose();
  }
}

void Tetris::reset() {
  for (int r = 0; r < GRID_HEIGHT; r++) {
    for (int c = 0; c < GRID_WIDTH; c++) {
      grid[r][c] = -1;
    }
  }
  nextType = getRandomType();
  heldPieceType = -1;
  spawnNewPiece();
  startTime = SDL_GetTicks();
  linesLeft = LINES_LEFT;
  gameOver = false;
  SoundManager::getInstance().startMainTheme();
}

bool Tetris::isColliding(std::vector<std::vector<int>>& piece,
                         int pieceRow,
                         int pieceCol) {
  for (int r = 0; r < piece.size(); r++) {
    for (int c = 0; c < piece[0].size(); c++) {
      if (piece[r][c] == 0) {
        continue;
      }

      int gr = pieceRow + r;
      int gc = pieceCol + c;
      if (gc < 0 || gc >= GRID_WIDTH || gr >= GRID_HEIGHT ||
          grid[gr][gc] >= 0) {
        return true;
      }
    }
  }
  return false;
}

void Tetris::addCurrentPiece() {
  for (int r = 0; r < currentPiece.size(); r++) {
    for (int c = 0; c < currentPiece[0].size(); c++) {
      if (currentPiece[r][c]) {
        int gr = curR + r;
        int gc = curC + c;
        grid[gr][gc] = curType;
      }
    }
  }
}

void Tetris::clearLines() {
  bool lineFull;
  int lowestFullRow = GRID_HEIGHT - 1;
  for (int r = GRID_HEIGHT - 1; r >= 0; r--) {
    lineFull = true;
    for (int c = 0; c < GRID_WIDTH; c++) {
      if (grid[r][c] < 0) {
        lineFull = false;
        break;
      }
    }
    if (lineFull) {
      lowestFullRow = r;
      break;
    }
  }

  // found lowest row that's full, so start clearing lines above
  while (lineFull) {
    for (int nr = lowestFullRow; nr > 0; nr--) {
      grid[nr] = grid[nr - 1];
    }
    linesLeft -= 1;
    linesLeft = std::max(linesLeft, 0);
    grid[0] = std::vector<int>(GRID_WIDTH, -1);
    for (int c = 0; c < GRID_WIDTH; c++) {
      if (grid[lowestFullRow][c] < 0) {
        lineFull = false;
        break;
      }
    }
  }

  if (linesLeft == 0) {
    gameOver = true;
    finishTime = SDL_GetTicks();
    gameOverText = "YOU WIN! - Press R to restart";
    SoundManager::getInstance().playYay();
  }
}

std::vector<std::vector<int>> Tetris::getWallKickData(int curRotation,
                                                      int nextRotation) {
  // I piece
  if (curType == 0) {
    if ((curRotation == 0 && nextRotation == 1) ||
        (curRotation == 3 && nextRotation == 2)) {
      return WALL_KICK_I[0];
    } else if ((curRotation == 1 && nextRotation == 0) ||
               (curRotation == 2 && nextRotation == 3)) {
      return WALL_KICK_I[1];
    } else if ((curRotation == 1 && nextRotation == 2) ||
               (curRotation == 0 && nextRotation == 3)) {
      return WALL_KICK_I[2];
    } else if ((curRotation == 2 && nextRotation == 1) ||
               (curRotation == 3 && nextRotation == 0)) {
      return WALL_KICK_I[3];
    }
    // none I piece
  } else {
    if ((curRotation == 0 && nextRotation == 1) ||
        (curRotation == 2 && nextRotation == 1)) {
      return WALL_KICK_NONE_I[0];
    } else if ((curRotation == 1 && nextRotation == 0) ||
               (curRotation == 1 && nextRotation == 2)) {
      return WALL_KICK_NONE_I[1];
    } else if ((curRotation == 2 && nextRotation == 3) ||
               (curRotation == 0 && nextRotation == 3)) {
      return WALL_KICK_NONE_I[2];
    } else if ((curRotation == 3 && nextRotation == 2) ||
               (curRotation == 3 && nextRotation == 0)) {
      return WALL_KICK_NONE_I[3];
    }
  }
  // should never get here
  std::cout << curRotation << ", " << nextRotation << std::endl;
  return {{0, 0}};
}

void Tetris::rotateClockwise() {
  std::vector<std::vector<int>> rotated;
  int n = currentPiece.size();
  int m = currentPiece[0].size();

  rotated.resize(m, std::vector<int>(n));

  for (int r = 0; r < n / 2; r++) {
    for (int c = 0; c < m; c++) {
      std::swap(currentPiece[r][c], currentPiece[n - r - 1][c]);
    }
  }

  for (int r = 0; r < n; r++) {
    for (int c = 0; c < m; c++) {
      rotated[c][r] = currentPiece[r][c];
    }
  }

  int nextRotation = (curRotation + 1) % 4;

  std::vector<std::vector<int>> kicks =
      getWallKickData(curRotation, nextRotation);

  for (auto& offset : kicks) {
    int offsetR = -offset[1];
    int offsetC = offset[0];
    if (!isColliding(rotated, curR + offsetR, curC + offsetC)) {
      currentPiece = std::move(rotated);
      curR = curR + offsetR;
      curC = curC + offsetC;
      curRotation = nextRotation;
      return;
    }
  }

  for (int r = 0; r < n / 2; r++) {
    for (int c = 0; c < m; c++) {
      std::swap(currentPiece[r][c], currentPiece[n - r - 1][c]);
    }
  }
}

void Tetris::rotateCounterClockwise() {
  std::vector<std::vector<int>> rotated;
  int n = currentPiece.size();
  int m = currentPiece[0].size();

  rotated.resize(m, std::vector<int>(n));

  for (int r = 0; r < n; r++) {
    for (int c = 0; c < m; c++) {
      rotated[c][r] = currentPiece[r][c];
    }
  }

  for (int r = 0; r < m / 2; r++) {
    for (int c = 0; c < n; c++) {
      std::swap(rotated[r][c], rotated[m - r - 1][c]);
    }
  }

  int nextRotation = (curRotation - 1 + 4) % 4;

  std::vector<std::vector<int>> kicks =
      getWallKickData(curRotation, nextRotation);

  for (auto& offset : kicks) {
    int offsetR = -offset[1];
    int offsetC = offset[0];
    if (!isColliding(rotated, curR + offsetR, curC + offsetC)) {
      currentPiece = std::move(rotated);
      curR = curR + offsetR;
      curC = curC + offsetC;
      curRotation = nextRotation;
      return;
    }
  }
}

void Tetris::dropPiece() {
  while (!isColliding(currentPiece, curR + 1, curC) && curR < GRID_HEIGHT) {
    curR++;
  }

  addCurrentPiece();
  clearLines();
  spawnNewPiece();
}

void Tetris::progressPieces() {
  if (gameOver) {
    return;
  }

  if (isColliding(currentPiece, curR + 1, curC)) {
    addCurrentPiece();
    clearLines();
    spawnNewPiece();
  } else {
    curR++;
  }
}

void Tetris::moveLeft() {
  if (!isColliding(currentPiece, curR, curC - 1)) {
    curC--;
  }
}

void Tetris::moveRight() {
  if (!isColliding(currentPiece, curR, curC + 1)) {
    curC++;
  }
}

void Tetris::render(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // draw existing grid
  for (int r = 0; r < GRID_HEIGHT; r++) {
    for (int c = 0; c < GRID_WIDTH; c++) {
      if (grid[r][c] >= 0) {
        SDL_Rect rect = {c * BLOCK_SIZE + GRID_OFFSET_X,
                         r * BLOCK_SIZE + GRID_OFFSET_Y, BLOCK_SIZE,
                         BLOCK_SIZE};
        SDL_Color color;
        if (gameOver) {
          color = {128, 128, 128, 255};
        } else {
          color = COLORS[grid[r][c]];
        }
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
      } else {
        SDL_Rect rect = {c * BLOCK_SIZE + GRID_OFFSET_X,
                         r * BLOCK_SIZE + GRID_OFFSET_Y, BLOCK_SIZE,
                         BLOCK_SIZE};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 32);
        SDL_RenderDrawRect(renderer, &rect);
      }
    }
  }

  // Draw piece in play

  if (!gameOver) {
    for (int r = 0; r < currentPiece.size(); r++) {
      for (int c = 0; c < currentPiece[0].size(); c++) {
        if (currentPiece[r][c] > 0) {
          SDL_Rect rect = {(curC + c) * BLOCK_SIZE + GRID_OFFSET_X,
                           (curR + r) * BLOCK_SIZE + GRID_OFFSET_Y, BLOCK_SIZE,
                           BLOCK_SIZE};
          SDL_Color color = COLORS[curType];
          SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
          SDL_RenderFillRect(renderer, &rect);
        }
      }
    }

    // draw ghost piece
    int ghostRow = curR;
    while (!isColliding(currentPiece, ghostRow + 1, curC) &&
           ghostRow < GRID_HEIGHT) {
      ghostRow++;
    }

    for (int r = 0; r < currentPiece.size(); r++) {
      for (int c = 0; c < currentPiece[0].size(); c++) {
        if (currentPiece[r][c] > 0) {
          SDL_Rect rect = {(curC + c) * BLOCK_SIZE + GRID_OFFSET_X,
                           (ghostRow + r) * BLOCK_SIZE + GRID_OFFSET_Y,
                           BLOCK_SIZE, BLOCK_SIZE};
          SDL_Color color = COLORS[curType];
          SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
          SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 128);
          SDL_RenderFillRect(renderer, &rect);
        }
      }
    }

    // held piece

    if (heldPieceType >= 0) {
      std::vector<std::vector<int>> heldPiece = BLOCKS[heldPieceType];
      for (int r = 0; r < heldPiece.size(); r++) {
        for (int c = 0; c < heldPiece[0].size(); c++) {
          if (heldPiece[r][c] > 0) {
            SDL_Rect rect = {c * BLOCK_SIZE + 40,
                             r * BLOCK_SIZE + GRID_OFFSET_Y, BLOCK_SIZE,
                             BLOCK_SIZE};
            SDL_Color color = COLORS[heldPieceType];
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
                                   color.a);
            SDL_RenderFillRect(renderer, &rect);
          }
        }
      }
    }

    // next piece
    int nextOffsetX = BLOCK_SIZE * GRID_WIDTH + GRID_OFFSET_X + 40;
    int nextOffsetY = GRID_OFFSET_Y;
    FontManager::getInstance().renderText(nextOffsetX, nextOffsetY, "Next", 0);
    nextOffsetY += 48;

    std::vector<std::vector<int>> nextPiece = BLOCKS[nextType];
    for (int r = 0; r < nextPiece.size(); r++) {
      for (int c = 0; c < nextPiece[0].size(); c++) {
        if (nextPiece[r][c] > 0) {
          SDL_Rect rect = {c * BLOCK_SIZE + nextOffsetX,
                           r * BLOCK_SIZE + nextOffsetY, BLOCK_SIZE,
                           BLOCK_SIZE};
          SDL_Color color = COLORS[nextType];
          SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
          SDL_RenderFillRect(renderer, &rect);
        }
      }
    }
  } else {
    auto textSize = FontManager::getInstance().getTextSize(gameOverText, 0);
    // draw game over text
    FontManager::getInstance().renderText(
        GRID_OFFSET_X, GRID_OFFSET_Y - textSize.second - 10, gameOverText, 0);
  }

  // text to always draw regardless of game state
  auto instructionsSize =
      FontManager::getInstance().getTextSize(INSTRUCTIONS, 0);
  int textX = GRID_WIDTH * BLOCK_SIZE + GRID_OFFSET_X + 30;
  int textY =
      GRID_HEIGHT * BLOCK_SIZE + GRID_OFFSET_Y - instructionsSize.second;
  FontManager::getInstance().renderText(textX, textY, INSTRUCTIONS, 0);
  textY -= instructionsSize.second;

  uint32_t elapsedTime;

  if (gameOver) {
    elapsedTime = finishTime - startTime;
  } else {
    elapsedTime = SDL_GetTicks() - startTime;
  }
  std::string timeString = formatMilliseconds(elapsedTime);
  auto timeSize = FontManager::getInstance().getTextSize(timeString, 0);
  FontManager::getInstance().renderText(textX, textY, timeString, 0);
  textY -= timeSize.second;

  std::string linesLeftText = std::format("Lines left: {}", linesLeft);
  FontManager::getInstance().renderText(textX, textY, linesLeftText, 0);
}

void Tetris::handleInput(const SDL_Event& event) {
  if (gameOver) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
      reset();
    }
    return;
  }

  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
      case SDLK_LEFT:
        if (!leftPressed) {
          moveLeft();
          leftPressed = true;
          leftTimer = SDL_GetTicks() + DAS_DELAY;
        }
        break;
      case SDLK_RIGHT:
        if (!rightPressed) {
          moveRight();
          rightPressed = true;
          rightTimer = SDL_GetTicks() + DAS_DELAY;
        }
        break;
      case SDLK_UP:
        rotateClockwise();
        SoundManager::getInstance().playRotate();
        break;
      case SDLK_z:
        rotateCounterClockwise();
        SoundManager::getInstance().playRotate();
        break;
      case SDLK_r:
        reset();
        break;
      case SDLK_c:
        if (canSwap) {
          if (heldPieceType == -1) {
            heldPieceType = curType;
            spawnNewPiece();
          } else {
            // swap held piece with current piece
            int nextHeld = curType;
            spawnNewPiece(heldPieceType);
            heldPieceType = nextHeld;
          }
          lastUpdate = SDL_GetTicks();
          canSwap = false;
        }
        break;
      case SDLK_DOWN:
        progressPieces();
        if (!downPressed) {
          downPressed = true;
          downTimer = SDL_GetTicks() + DAS_DELAY;
        } else {
          lastUpdate = SDL_GetTicks();
        }
        break;
      case SDLK_SPACE:
        dropPiece();
        SoundManager::getInstance().playDrop();
        lastUpdate = SDL_GetTicks();
        break;
    }
  }

  if (event.type == SDL_KEYUP) {
    switch (event.key.keysym.sym) {
      case SDLK_LEFT:
        leftPressed = false;
        break;
      case SDLK_RIGHT:
        rightPressed = false;
        break;
      case SDLK_DOWN:
        downPressed = false;
        break;
    }
  }
}

void Tetris::update() {
  if (gameOver) {
    return;
  }

  Uint32 currentTime = SDL_GetTicks();
  // If the piece is colliding below, give the user extra time to make rotation
  Uint32 update_delay = isColliding(currentPiece, curR + 1, curC)
                            ? LAST_ROW_UPDATE_DELAY
                            : UPDATE_DELAY;

  if (currentTime - lastUpdate >= update_delay) {
    progressPieces();
    lastUpdate = currentTime;
  }
  if (!rightPressed && leftPressed && currentTime >= leftTimer) {
    moveLeft();
    leftTimer = currentTime + DAS_REPEAT;
  }
  if (!leftPressed && rightPressed && currentTime >= rightTimer) {
    moveRight();
    rightTimer = currentTime + DAS_REPEAT;
  }

  if (downPressed && !isColliding(currentPiece, curR + 1, curC) &&
      currentTime >= downTimer) {
    progressPieces();
    downTimer = currentTime + DAS_REPEAT;
  }
}
