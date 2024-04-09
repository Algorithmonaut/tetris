#include "main.h"
#include "tetrimino.h"
#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

////////////////////////////////////////////////////////////////////////////////
//// Init graphics

void initColorPairs() {
  init_pair(BLACK, COLOR_BLACK, COLOR_BLACK);
  init_pair(RED, COLOR_RED, COLOR_BLACK);
  init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);
}

void initWindows() {
  int centerX = COLS / 2;
  int centerY = LINES / 2;

  {
    const int x = centerX - GRID_COLS;
    const int y = centerY - GRID_ROWS / 2;
    const int width = GRID_COLS * 2 + 2;
    const int height = GRID_ROWS + 2;
    gridWindow = newwin(height, width, y, x);
    box(gridWindow, 0, 0);
  }

  {
    const int x = centerX + GRID_COLS + 4;
    const int y = centerY - GRID_ROWS / 2;
    const int width = 4 * 2 + 2;
    const int height = 4 + 2;
    previewWindow = newwin(height, width, y, x);
  }

  {
    const int x = centerX + GRID_COLS + 4;
    const int y = centerY - GRID_ROWS / 2 + 5;
    const int width = 8 + 2;
    const int height = 1 + 2;
    scoreWindow = newwin(height, width, y, x);
    box(scoreWindow, 0, 0);
  }
}

void initGraphics() {
  initscr();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(FALSE);
  // timeout(10);
  start_color();

  atexit((void (*)(void))endwin);

  initColorPairs();
  initWindows();
  refresh();
}

////////////////////////////////////////////////////////////////////////////////
//// Init

void initCtx() {
  ctx.touchdown = FALSE;
  ctx.isKeyPressed = FALSE;
  ctx.score = 0;
  nextCurrentTetrimino();
}

void initGrid() {
  for (int row = 0; row < GRID_ROWS; ++row) {
    for (int col = 0; col < GRID_COLS; ++col) {
      grid[row][col] = 0;
    }
  }
}

void initAll() {
  srand(time(NULL));
  setlocale(LC_ALL, "");

  initCtx();
  initGrid();
  initGraphics();
}
