#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <ncurses.h>
#include <wchar.h>

// Uncomment for using nerd font characters
// #define USE_NERD_FONT

//// Colors

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

//// CTX

typedef struct {
  Color color;
  int model[4][4];
} Tetromino;

typedef struct {
  Tetromino *tetromino;
  int posCol;
  int posRow;
  int rotation;
} ActiveTetromino;

typedef struct {
  unsigned long long score;
  int difficulty;
  ActiveTetromino current;
  Tetromino *next;
  int touchdown;
  int isKeyPressed;
} CTX;

extern CTX ctx;

//// Grid

#define GRID_ROWS 23
#define GRID_COLS 10
extern int grid[GRID_ROWS][GRID_COLS];

//// Tetromino

#define TETROMINOS_COUNT 7

//// Victory condition

#define TRUE 1
#define FALSE 0
#define DONE 2

//// Windows

extern WINDOW *gridWindow;
extern WINDOW *previewWindow;
extern WINDOW *scoreWindow;

//// Vectors

typedef struct {
  int col;
  int row;
} Vec2i;

#endif
