#include "core.h"
#include "main.h"
#include <stdbool.h>
#include <stdlib.h>

// Model holds the models of the differents tetriminos, while rows represent
// the differents rotation, and cols the different index of a pixel on a 4x4
// matrix

// Index
// +--+---+---+---+
// |0 | 1 | 2 | 3 |
// +--+---+---+---+
// |4 | 5 | 6 | 7 |
// +--+---+---+---+
// |8 | 9 | 10| 11|
// +--+---+---+---+
// |12| 13| 14| 15|
// +--+---+---+---+

Tetrimino tetriminoI = {
    CYAN, {{4, 5, 6, 7}, {2, 6, 10, 14}, {8, 9, 10, 11}, {1, 5, 9, 13}}};
Tetrimino tetriminoO = {
    YELLOW, {{1, 2, 5, 6}, {1, 2, 5, 6}, {1, 2, 5, 6}, {1, 2, 5, 6}}};
Tetrimino tetriminoT = {
    MAGENTA, {{4, 5, 1, 6}, {1, 5, 6, 9}, {4, 5, 9, 6}, {1, 5, 4, 9}}};
Tetrimino tetriminoJ = {
    BLUE, {{0, 4, 5, 6}, {2, 1, 5, 9}, {10, 6, 5, 4}, {8, 9, 5, 1}}};
Tetrimino tetriminoL = {
    WHITE, {{2, 6, 5, 4}, {10, 9, 5, 1}, {8, 4, 5, 6}, {0, 1, 5, 9}}};
Tetrimino tetriminoS = {
    GREEN, {{4, 5, 1, 2}, {1, 5, 6, 10}, {8, 9, 5, 6}, {0, 4, 5, 9}}};
Tetrimino tetriminoZ = {
    RED, {{0, 1, 5, 6}, {9, 5, 6, 2}, {4, 5, 9, 10}, {8, 4, 5, 1}}};

Tetrimino *tetriminos[TETRIMINOS_COUNT] = {
    &tetriminoI, &tetriminoO, &tetriminoT, &tetriminoJ,
    &tetriminoL, &tetriminoS, &tetriminoZ};

// Check for collision (of either the current rotation or the next rotation)
// using the current tetromino and the translation vector
bool isCurrentColliding(int translateRow, int translateCol, bool nextRotation) {
  for (int i = 0; i < 4; ++i) {
    Vec2i pixelPos;
    if (nextRotation) {
      pixelPos = getNextPixelPos(i);
    } else {
      pixelPos = getCurrentPixelPos(i);
    }

    pixelPos.row += translateRow + ctx.current.posRow;
    pixelPos.col += translateCol + ctx.current.posCol;

    if (pixelPos.row >= GRID_ROWS || pixelPos.col == 0 ||
        pixelPos.col >= GRID_COLS || grid[pixelPos.row][pixelPos.col] != 0) {
      return true;
    }
  }

  return false;
}

void nextRotation() {
  ctx.current.rotation == 3 ? ctx.current.rotation = 0 : ++ctx.current.rotation;
}

void moveLeft() {
  if (!isCurrentColliding(0, -1, false)) {
    --ctx.current.posCol;
  }
}
void moveRight() {
  if (!isCurrentColliding(0, -1, false)) {
    ++ctx.current.posCol;
  }
}
void moveBottom() {}

void rotate() {
  if (!isCurrentColliding(0, 0, true)) {
    nextRotation();
  }
}

void nextCurrentTetrimino() {
  ctx.current.tetrimino = ctx.next;
  ctx.next = tetriminos[rand() % TETRIMINOS_COUNT];
  ctx.current.posCol = GRID_COLS / 4 + 1;
  ctx.current.posRow = 0;
  ctx.current.rotation = 0;
}
