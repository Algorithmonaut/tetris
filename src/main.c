
#include <complex.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

// Simple implementation of tetris in the cli
// https://en.wikipedia.org/wiki/Tetromino

// Implement SRS

/*
#define ANSI_COLOR_BLACK "\x1b[30m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_RESET "\x1b[0m"
*/

#define TETROMINO_ROWS 4
#define TETROMINO_COLS 4

#define GRID_ROWS 10
#define GRID_COLS 20
int grid[GRID_ROWS][GRID_COLS];

#define TRUE 1
#define FALSE 0

void initGrid() {
  for (int row = 0; row < GRID_ROWS; ++row)
    for (int col = 0; col < GRID_COLS; ++col)
      grid[row][col] = 0;
}

typedef enum {
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
  int model[TETROMINO_ROWS][TETROMINO_COLS];
} Tetromino;

Tetromino tetrominoI = {
    CYAN, {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}};
Tetromino tetrominoO = {
    YELLOW, {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}};
Tetromino tetrominoT = {
    MAGENTA, {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};
Tetromino tetrominoJ = {
    BLUE, {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}};
Tetromino tetrominoL = {
    WHITE, {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}};
Tetromino tetrominoS = {
    GREEN, {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};
Tetromino tetrominoZ = {
    RED, {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};

void printTetromino(Tetromino tetromino) {

  // wchar_t unicode_char = L'█';
  wchar_t *unicode_char = L"󰝤 ";

  for (int row = 0; row < TETROMINO_ROWS; ++row) {
    for (int col = 0; col < TETROMINO_COLS; ++col) {
      if (tetromino.model[row][col] == 1) {
        // wprintw(stdscr, "%lc ", unicode_char);
        printw()
      } else {
        printw("  ");
      }
    }
    printw("\n");
  }

  refresh();
}

int main() {
  Tetromino *tetrominos[7] = {&tetrominoI, &tetrominoO, &tetrominoT,
                              &tetrominoJ, &tetrominoL, &tetrominoS,
                              &tetrominoZ};

  initscr();
  cbreak(); // Disable line buffering and allow eachy key press to be
            // immediately available
  noecho(); // Don't echo key presses
  keypad(stdscr, TRUE); // Enable special keys such as arrow keys

  initGrid();

  setlocale(LC_ALL, "");

  for (int i = 0; i < 7; ++i) {
    printTetromino(*tetrominos[i]);
  }

  return EXIT_SUCCESS;
}
