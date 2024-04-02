#include "definitions.h"

void updateScore(int lines) {
  switch (lines) {
  case 1:
    ctx.score += 40;
    break;
  case 2:
    ctx.score += 100;
    break;
  case 3:
    ctx.score += 300;
    break;
  case 4:
    ctx.score += 1200;
    break;
  }
}

void processLines() {
  int linesAccumulator = 0;
  for (int row = GRID_ROWS - 1; row >= 0; --row) {
    int accumulator = 0;
    for (int col = 0; col < GRID_COLS; ++col) {
      if (grid[row][col] != 0) {
        ++accumulator;
      }
    }

    if (accumulator == GRID_COLS) {
      ++linesAccumulator;
      for (int movedRow = row; movedRow > 0; --movedRow) {
        for (int col = 0; col < GRID_COLS; ++col) {
          grid[movedRow][col] = grid[movedRow - 1][col];
        }
      }
    }
  }

  updateScore(linesAccumulator);

  /*
  for (int col = 0; col < GRID_COLS; ++col) {
    if (grid[3][col] != 0) {
      endwin();
      printf("\n\n%lli\n\n", ctx.score);
      exit(0);
    }
  }
  */
}
