#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <ncurses.h>
#include <wchar.h>

// If not defined, unicode characters will be used instead
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
} Tetrimino;

typedef struct {
  Tetrimino *tetrimino;
  int posCol;
  int posRow;
  int rotation;
} ActiveTetrimino;

typedef struct {
  unsigned long long score;
  int difficulty;
  ActiveTetrimino current;
  Tetrimino *next;
  int touchdown;
  int isKeyPressed;
  int levelSpeed;
} CTX;

extern CTX ctx;

//// Grid

#define GRID_ROWS 23
#define GRID_COLS 10
extern int grid[GRID_ROWS][GRID_COLS];

//// Tetrimino

#define TETRIMINOS_COUNT 7

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
