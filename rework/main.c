#include "main.h"
#include "init.h"
#include "renderer.h"
#include "tetrimino.h"
#include <bits/types/struct_timeval.h>
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

CTX ctx;

int grid[GRID_ROWS][GRID_COLS];

WINDOW *gridWindow;
WINDOW *previewWindow;
WINDOW *scoreWindow;

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

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage:\n\t %s <fall per sec>\n", argv[0]);
    return 1;
  }
  ctx.levelSpeed = atoi(argv[1]);

  initAll();

  struct timeval startTime, currentTime;
  long elapsedTime;
  gettimeofday(&startTime, NULL);

  while (TRUE) {
    gettimeofday(&currentTime, NULL);
    elapsedTime = (currentTime.tv_sec - startTime.tv_sec) * 1000000 +
                  (currentTime.tv_usec - startTime.tv_usec);

    if (elapsedTime >= 1000000 / ctx.levelSpeed) {
      if (ctx.isKeyPressed == FALSE && ctx.touchdown == TRUE) {
        ctx.touchdown = DONE;
      }
    }

    renderAll();
  }

  return 0;
}
