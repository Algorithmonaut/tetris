
#include <complex.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

// Simple implementation of tetris in the cli
// https://en.wikipedia.org/wiki/Tetromino

// Implement SRS

////////////////////////////////////////////////////////////////////////////////
//// Definitions

#define ANSI_COLOR_BLACK "\x1b[30m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define TETROMINO_ROWS 4
#define TETROMINO_COLS 4

#define GRID_ROWS 20
#define GRID_COLS 10
int grid[GRID_ROWS][GRID_COLS];

#define TETROMINOS_COUNT 7

#define TRUE 1
#define FALSE 0

WINDOW *gridWindow;

static const chtype BLOCK = ' ' | A_REVERSE;

typedef enum {
  DEFAULT,
  BLACK,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE,
} Color;

typedef struct {
  Color color;
  int model[4][4];
} Tetromino;

typedef struct {
  Tetromino *tetromino;
  int posX;
  int posY;
  int rotation;
} ActiveTetromino;

typedef struct {
  unsigned long long score;
  int difficulty;
  ActiveTetromino current;
} CTX;

CTX ctx;

////////////////////////////////////////////////////////////////////////////////
//// Init

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

void initGrid() {
  for (int row = 0; row < GRID_ROWS; ++row) {
    for (int col = 0; col < GRID_COLS; ++col) {
      grid[row][col] = 0;
    }
  }
}

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
}

////////////////////////////////////////////////////////////////////////////////
//// Helpers

Tetromino *selectTetromino() { return tetrominos[rand() % TETROMINOS_COUNT]; }

int getCurrentYPixelPos(int pixel) {
  return ctx.current.tetromino->model[ctx.current.rotation][pixel] / 4;
}
int getCurrentXPixelPos(int pixel) {
  return ctx.current.tetromino->model[ctx.current.rotation][pixel] % 4;
}

int getRotatedXPixelPos(int pixel) {
  int rotation = ctx.current.rotation == 3 ? 0 : ctx.current.rotation + 1;
  return ctx.current.tetromino->model[rotation][pixel] % 4;
}
int getRotatedYPixelPos(int pixel) {
  int rotation = ctx.current.rotation == 3 ? 0 : ctx.current.rotation + 1;
  return ctx.current.tetromino->model[rotation][pixel] / 4;
}

////////////////////////////////////////////////////////////////////////////////
//// Fuzzy print

void printGrid() {
  for (int row = 0; row < GRID_ROWS; row++) {
    for (int col = 0; col < GRID_COLS; col++) {
      if (grid[row][col] != 0) {
        wattron(gridWindow, COLOR_PAIR(grid[row][col]));
        mvwaddch(gridWindow, row + 1, col * 2 + 1, BLOCK);
        mvwaddch(gridWindow, row + 1, col * 2 + 2, BLOCK);
        wattroff(gridWindow, COLOR_PAIR(grid[row][col]));
      } else {
        mvwaddch(gridWindow, row + 1, col * 2 + 1, ' ');
        mvwaddch(gridWindow, row + 1, col * 2 + 2, ' ');
      }
    }
  }

  wrefresh(gridWindow);
}

void printTetrominoOnGrid(void) {
  wattron(gridWindow, COLOR_PAIR(ctx.current.tetromino->color));

  int x = ctx.current.posX * 2;
  int y = ctx.current.posY;

  for (int i = 0; i < 4; ++i) {
    int row = getCurrentYPixelPos(i);
    int col = getCurrentXPixelPos(i);

    mvwaddch(gridWindow, y + row + 1, x + col * 2 + 1, BLOCK);
    mvwaddch(gridWindow, y + row + 1, x + col * 2 + 2, BLOCK);
  }

  wattroff(gridWindow, COLOR_PAIR(ctx.current.tetromino->color));
  wrefresh(gridWindow);
}

////////////////////////////////////////////////////////////////////////////////
//// Main

void initGraphics(void) {
  initscr();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(FALSE);
  timeout(100);

  atexit((void (*)(void))endwin);

  // use_default_colors();
  start_color();
  initColorPairs();

  initGrid();

  refresh();
  initWindows();
  printGrid();
}

void moveLeft() {
  for (int i = 0; i < 4; ++i) {
    int col = getCurrentXPixelPos(i) + ctx.current.posX;
    if (col == 0) {
      return;
    }
  }

  --ctx.current.posX;
}
void moveRight() {
  for (int i = 0; i < 4; ++i) {
    int col = getCurrentXPixelPos(i) + ctx.current.posX;
    if (col == GRID_COLS - 1) {
      return;
    }
  }

  ++ctx.current.posX;
}

void rotate() {
  for (int i = 0; i < 4; ++i) {
    int col = getRotatedXPixelPos(i) + ctx.current.posX + 1;

    if (col == GRID_COLS || col == 0) {
      return;
    }
  }

  ctx.current.rotation == 3 ? ctx.current.rotation = 0 : ++ctx.current.rotation;
};
void moveBottom(){};

void handleInputs() {
  switch (getch()) {
  case 'h':
  case KEY_LEFT:
    moveLeft();
    break;
  case 'l':
  case KEY_RIGHT:
    moveRight();
    break;
  case 'k':
  case KEY_UP:
    rotate();
    break;
  case ' ':
    moveBottom();
    break;
  case 'q':
    exit(0);
  }
}

void updateCurrentTetromino() {
  ctx.current.tetromino = selectTetromino();
  ctx.current.posX = GRID_COLS / 4 + 1;
  ctx.current.posY = 0;
  ctx.current.rotation = 0;
}

int main(void) {
  srand(time(NULL));

  initGraphics();

  updateCurrentTetromino();

  /*
  for (int i = 0; i < TETROMINOS_COUNT; ++i) {
    printTetromino(*tetrominos[i]);
  }
  */

  // int nextTetromino = selectTetromino();
  // int currentTetromino = selectTetromino();

  const int levelSpeed = 1;

  clock_t startTime, currentTime;
  double elapsedTime;

  startTime = clock();

  while (TRUE) {
    currentTime = clock();
    elapsedTime = (double)(currentTime - startTime) / CLOCKS_PER_SEC;

    handleInputs();
    printGrid();
    printTetrominoOnGrid();

    if (elapsedTime * 1000 >= (float)1 / levelSpeed) {
      ++ctx.current.posY;
      startTime = currentTime;
    }

    // currentTetromino = nextTetromino;
    // nextTetromino = selectTetromino();
  }

  // endwin();

  return EXIT_SUCCESS;
}
