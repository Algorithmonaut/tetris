#include "core.h"
#include "definitions.h"
#include <stdlib.h>

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
