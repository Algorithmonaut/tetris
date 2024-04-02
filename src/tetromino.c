#include "core.h"
#include "definitions.h"
#include <stdlib.h>

// Model holds the models of the differents tetrominos, while rows represent
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

Tetromino tetrominoI = {
    CYAN, {{4, 5, 6, 7}, {2, 6, 10, 14}, {8, 9, 10, 11}, {1, 5, 9, 13}}};
Tetromino tetrominoO = {
    YELLOW, {{1, 2, 5, 6}, {1, 2, 5, 6}, {1, 2, 5, 6}, {1, 2, 5, 6}}};
Tetromino tetrominoT = {
    MAGENTA, {{4, 5, 1, 6}, {1, 5, 6, 9}, {4, 5, 9, 6}, {1, 5, 4, 9}}};
Tetromino tetrominoJ = {
    BLUE, {{0, 4, 5, 6}, {2, 1, 5, 9}, {10, 6, 5, 4}, {8, 9, 5, 1}}};
Tetromino tetrominoL = {
    WHITE, {{2, 6, 5, 4}, {10, 9, 5, 1}, {8, 4, 5, 6}, {0, 1, 5, 9}}};
Tetromino tetrominoS = {
    GREEN, {{4, 5, 1, 2}, {1, 5, 6, 10}, {8, 9, 5, 6}, {0, 4, 5, 9}}};
Tetromino tetrominoZ = {
    RED, {{0, 1, 5, 6}, {9, 5, 6, 2}, {4, 5, 9, 10}, {8, 4, 5, 1}}};

Tetromino *tetrominos[TETROMINOS_COUNT] = {
    &tetrominoI, &tetrominoO, &tetrominoT, &tetrominoJ,
    &tetrominoL, &tetrominoS, &tetrominoZ};

void nextCurrentTetromino() {
  ctx.current.tetromino = ctx.next;
  ctx.next = tetrominos[rand() % TETROMINOS_COUNT];
  ctx.current.posCol = GRID_COLS / 4 + 1;
  ctx.current.posRow = 0;
  ctx.current.rotation = 0;
}

////////////////////////////////////////////////////////////////////////////////
//// Movements

int checkYOffset(int offset) {
  for (int i = 0; i < 4; ++i) {
    Vec2i pixelPos = getCurrentPixelPos(i);
    pixelPos.row += ctx.current.posRow;
    pixelPos.col += ctx.current.posCol + offset;

    if (pixelPos.col == 0 || pixelPos.col == GRID_COLS - 1 ||
        grid[pixelPos.row][pixelPos.col] != 0) {
      return FALSE;
    }
  }
  return TRUE;
}

void moveLeft() {
  if (checkYOffset(-1)) {
    --ctx.current.posRow;
  }
}

void moveRight() {
  if (checkYOffset(+1)) {
    ++ctx.current.posRow;
  }
}

void rotate() {
  for (int i = 0; i < 4; ++i) {
    Vec2i rotatedPixelPos = getRotatedPixelPos(i);
    if (rotatedPixelPos.col == 0 || rotatedPixelPos.col == GRID_COLS - 1 ||
        grid[rotatedPixelPos.row][rotatedPixelPos.col] != 0) {
      return;
    }
  }

  ctx.current.rotation == 3 ? ctx.current.rotation = 0 : ++ctx.current.rotation;
}

void moveBottom() { ctx.current.posRow = getCurrrentFinalHeight(); }

void stickToGrid() {
  for (int i = 0; i < 4; ++i) {
    Vec2i pixelPos = getRotatedPixelPos(i);

    grid[pixelPos.row + ctx.current.posRow][pixelPos.col + ctx.current.posCol] =
        ctx.current.tetromino->color;
  }
}

/*
void moveBottom() {
  ctx.current.posY += getCurrentFinalHeight();

  stickTetrominoToGrid();
  updateCurrentTetromino();
}

 stickTetrominoToGrid();
  updateCurrentTetromino();
-> commitTetromino();
*/
