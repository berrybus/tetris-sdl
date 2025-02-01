#include "Tetris.h"
#include <memory>

const int BLOCK_SIZE = 30;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;

const Uint32 UPDATE_DELAY = 1000;

const uint32_t DAS_DELAY = 133;
const uint32_t DAS_REPEAT = 10;

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

Tetris::Tetris(SceneManager &sceneManager)
    : Scene(sceneManager), grid(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, -1)),
      gameOver(false), lastUpdate(SDL_GetTicks()) {
  SDL_Init(SDL_INIT_VIDEO);
  spawnNewPiece();
}

void Tetris::spawnNewPiece() {
  std::random_device rd; // a seed source for the random number engine
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(0, BLOCKS.size() - 1);
  curType = dist(gen);
  currentPiece = BLOCKS[curType];
  curC = GRID_WIDTH / 2 - currentPiece[0].size() / 2;
  curR = 0;
  if (isColliding(currentPiece, curR, curC)) {
    std::cout << "colliding in spawn new piece" << std::endl;
    gameOver = true;
  }
}

bool Tetris::isColliding(std::vector<std::vector<int>> &piece, int pieceRow,
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
    grid[0] = std::vector<int>(GRID_WIDTH, -1);
    for (int c = 0; c < GRID_WIDTH; c++) {
      if (grid[lowestFullRow][c] < 0) {
        lineFull = false;
        break;
      }
    }
  }
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

  if (!isColliding(rotated, curR, curC)) {
    currentPiece = std::move(rotated);
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

  if (!isColliding(rotated, curR, curC)) {
    currentPiece = std::move(rotated);
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

void Tetris::render(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (int r = 0; r < GRID_HEIGHT; r++) {
    for (int c = 0; c < GRID_WIDTH; c++) {
      if (grid[r][c] >= 0) {
        SDL_Rect rect = {c * BLOCK_SIZE, r * BLOCK_SIZE, BLOCK_SIZE,
                         BLOCK_SIZE};
        SDL_Color color = COLORS[grid[r][c]];
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
      }
    }
  }

  for (int r = 0; r < currentPiece.size(); r++) {
    for (int c = 0; c < currentPiece[0].size(); c++) {
      if (currentPiece[r][c] > 0) {
        SDL_Rect rect = {(curC + c) * BLOCK_SIZE, (curR + r) * BLOCK_SIZE,
                         BLOCK_SIZE, BLOCK_SIZE};
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
        SDL_Rect rect = {(curC + c) * BLOCK_SIZE, (ghostRow + r) * BLOCK_SIZE,
                         BLOCK_SIZE, BLOCK_SIZE};
        SDL_Color color = COLORS[curType];
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 128);
        SDL_RenderFillRect(renderer, &rect);
      }
    }
  }
}

void Tetris::handleInput(const SDL_Event &event) {
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
      break;
    case SDLK_z:
      rotateCounterClockwise();
      break;
    case SDLK_DOWN:
      progressPieces();
      lastUpdate = SDL_GetTicks();
      break;
    case SDLK_SPACE:
      dropPiece();
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
    }
  }
}

void Tetris::update() {
  Uint32 currentTime = SDL_GetTicks();
  if (currentTime - lastUpdate >= UPDATE_DELAY) {
    progressPieces();
    lastUpdate = currentTime;
  }
  if (leftPressed && currentTime >= leftTimer) {
    moveLeft();
    leftTimer = currentTime + DAS_REPEAT;
  }
  if (rightPressed && currentTime >= rightTimer) {
    moveRight();
    rightTimer = currentTime + DAS_REPEAT;
  }
}
