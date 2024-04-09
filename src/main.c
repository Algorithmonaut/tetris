#include <complex.h>
#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

#include "core.h"
#include "definitions.h"
#include "init.h"
#include "tetromino.h"

CTX ctx;

WINDOW *gridWindow;
WINDOW *previewWindow;
WINDOW *scoreWindow;

int grid[GRID_ROWS][GRID_COLS];

int getFinalHeight() {
  int finalHeight = GRID_ROWS;
  for (int i = 0; i < 4; ++i) {
    Vec2i pixelPos = getCurrentPixelPos(i);

    for (int row = ctx.current.posRow; row < GRID_ROWS - 3; ++row) {
      int globalRow = pixelPos.row + row;
      if (globalRow >= GRID_ROWS || grid[globalRow][pixelPos.col] != 0) {
        if (globalRow < finalHeight) {
          finalHeight = globalRow;
        }
      }
    }
  }

  return finalHeight;
}

// Check for collision (of either the current rotation or the next rotation)
// using the current tetromino and the translation vector
bool isColliding(int translateRow, int translateCol, bool nextRotation) {
  for (int i = 0; i < 4; ++i) {
    Vec2i pixelPos;
    if (nextRotation) {
      pixelPos = getRotatedPixelPos(i);
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

void checkCollision() {
  /*
  if (ctx.touchdown == DONE) {
    return;
  }
  */

  for (int i = 0; i < 4; ++i) {
    Vec2i pixelPos = getCurrentPixelPos(i);
    pixelPos.row += ctx.current.posRow;
    pixelPos.col += ctx.current.posCol;

    if (pixelPos.row >= GRID_ROWS || grid[pixelPos.row][pixelPos.col] != 0) {
      ctx.touchdown = TRUE;
      return;
    }
  }

  ctx.touchdown = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//// Player controls

void handleInputs() {
  switch (getch()) {
  case 'h':
  case KEY_LEFT:
    moveLeft();
    ctx.isKeyPressed = TRUE;
    break;
  case 'l':
  case KEY_RIGHT:
    moveRight();
    ctx.isKeyPressed = TRUE;
    break;
  case 'k':
  case KEY_DOWN:
    rotate();
    break;
  case ' ':
  case KEY_UP:
    moveBottom();
    break;
  case 'q':
    exit(0);
    break;
  }
}

////////////////////////////////////////////////////////////////////////////////
//// Main

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage:\n\t %s <fall per sec>\n", argv[0]);
    return 1;
  }

  initAll();

  int levelSpeed = atoi(argv[1]);

  {
    struct timeval startTime, currentTime;
    long elapsedTime;
    gettimeofday(&startTime, NULL);
  }

  while (TRUE) {
    gettimeofday(&currentTime, NULL);
    elapsedTime = (currentTime.tv_sec - startTime.tv_sec) * 1000000 +
                  (currentTime.tv_usec - startTime.tv_usec);

    if (elapsedTime >= 1000000 / levelSpeed) {
      if (ctx.isKeyPressed == FALSE && ctx.touchdown == TRUE) {
        ctx.touchdown = DONE;
      }

      ctx.isKeyPressed = FALSE;

      if (ctx.touchdown == DONE) {
        stickTetrominoToGrid();
        updateCurrentTetromino();
        ctx.touchdown = FALSE;
      }

      if (ctx.touchdown == FALSE) {
        ++ctx.current.posY;
      }
      startTime = currentTime;
    }

    handleInputs();
    printTetrominoPreviewOnGrid(getCurrentFinalHeight());
    printTetrominoOnGrid();

    checkTetrominoCollision();
    checkLines();
  }

  return EXIT_SUCCESS;
}
