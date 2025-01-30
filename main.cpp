#include <SDL2/SDL.h>

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <ctime>
#include <random>
#include <vector>

const int BLOCK_SIZE = 30;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;
const int SCREEN_WIDTH = GRID_WIDTH * BLOCK_SIZE;
const int SCREEN_HEIGHT = GRID_HEIGHT * BLOCK_SIZE;

const Uint32 UPDATE_DELAY = 750;

const std::vector<std::vector<std::vector<int>>> BLOCKS = {
    {{1, 1, 1, 1}},           {{1, 1}, {1, 1}},         {{0, 1, 0}, {1, 1, 1}},
    {{1, 0}, {1, 0}, {1, 1}}, {{0, 1}, {0, 1}, {1, 1}}, {{0, 1, 1}, {1, 1, 0}},
    {{1, 1, 0}, {0, 1, 1}}};

const SDL_Color COLORS[] = {{0, 255, 255, 255}, {255, 255, 0, 255},
                            {128, 0, 128, 255}, {255, 127, 0, 255},
                            {0, 0, 255, 255},   {0, 255, 0, 255},
                            {255, 0, 0, 255}};

class Tetris {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::vector<std::vector<int>> grid;
  std::vector<std::vector<int>> currentPiece;
  int curR;
  int curC;
  int curType;

public:
  bool gameOver;
  Tetris()
      : grid(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, -1)), gameOver(false) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    spawnNewPiece();
  }

  ~Tetris() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void spawnNewPiece() {
    std::random_device rd; // a seed source for the random number engine
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, BLOCKS.size() - 1);
    curType = dist(gen);
    currentPiece = BLOCKS[curType];
    curC = GRID_WIDTH / 2 - currentPiece[0].size() / 2;
    curR = 0;
  }

  bool isColliding(std::vector<std::vector<int>> &piece, int pieceRow,
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

  void addCurrentPiece() {
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

  void clearLines() {
    bool lineFull = true;
    int lowestFullRow = GRID_HEIGHT - 1;
    for (int r = GRID_HEIGHT - 1; r >= 0; r--) {
      for (int c = 0; c < GRID_WIDTH; c++) {
        if (grid[r][c] < 0) {
          lineFull = false;
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

  void rotateClockwise() {
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

  void update() {
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

  void moveLeft() {
    if (!isColliding(currentPiece, curR, curC - 1)) {
      curC--;
    }
  }

  void moveRight() {
    if (!isColliding(currentPiece, curR, curC + 1)) {
      curC++;
    }
  }

  void render() {
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
    SDL_RenderPresent(renderer);
  }
};

int main() {
  Tetris game;
  SDL_Event event;
  Uint32 lastUpdate = SDL_GetTicks();

  while (!game.gameOver) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        return 0;
      }
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_LEFT:
          game.moveLeft();
          break;
        case SDLK_RIGHT:
          game.moveRight();
          break;
        case SDLK_UP:
          game.rotateClockwise();
          break;
        case SDLK_DOWN:
          game.update();
          lastUpdate = SDL_GetTicks();
          break;
        }
      }
    }

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastUpdate >= UPDATE_DELAY) {
      game.update();
      lastUpdate = currentTime;
    }

    game.render();
  }

  return 0;
}
