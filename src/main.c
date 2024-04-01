
#include <complex.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

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

#define GRID_ROWS 23
#define GRID_COLS 10
int grid[GRID_ROWS][GRID_COLS];

#define TETROMINOS_COUNT 7

#define TRUE 1
#define FALSE 0
#define DONE 2

WINDOW *gridWindow;
WINDOW *previewWindow;

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
  Tetromino *future;
  int touchdown;
  int isKeyPressed;
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
  // init_pair(100, COLOR_BLACK, COLOR_WHITE);
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
}

void initGraphics(void) {
  initscr();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(FALSE);
  timeout(10);

  atexit((void (*)(void))endwin);

  // use_default_colors();
  start_color();
  initColorPairs();

  initGrid();

  refresh();
  initWindows();
}

wchar_t *square = L"󰝤";
wchar_t *wireSquare = L"󰝣";

////////////////////////////////////////////////////////////////////////////////
//// Helpers

Tetromino *selectTetromino() { return tetrominos[rand() % TETROMINOS_COUNT]; }

int getCurrentYPixelPos(int pixel) {
  return ctx.current.tetromino->model[ctx.current.rotation][pixel] / 4;
}
int getCurrentXPixelPos(int pixel) {
  return ctx.current.tetromino->model[ctx.current.rotation][pixel] % 4;
}

int getFutureYPixelPos(int pixel) { return ctx.future->model[0][pixel] / 4; }
int getFutureXPixelPos(int pixel) { return ctx.future->model[0][pixel] % 4; }

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
        mvwprintw(gridWindow, row + 1, col * 2 + 1, "%ls", square);
        wattroff(gridWindow, COLOR_PAIR(grid[row][col]));
      } else {
        // We still need to do that to clear the grid
        mvwaddch(gridWindow, row + 1, col * 2 + 1, ' ');
      }
    }
  }
}

void clearPreview() {
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      mvwaddch(previewWindow, row + 1, col * 2 + 1, ' ');
    }
  }
}

void printPreview() {
  clearPreview();
  for (int i = 0; i < 4; ++i) {
    int row = getFutureYPixelPos(i);
    int col = getFutureXPixelPos(i);
    wattron(previewWindow, COLOR_PAIR(ctx.future->color));
    mvwprintw(previewWindow, row + 1, col * 2 + 1, "%ls", square);
    wattroff(previewWindow, COLOR_PAIR(ctx.future->color));
  }
}

void printTetrominoOnGrid(void) {
  wattron(gridWindow, COLOR_PAIR(ctx.current.tetromino->color));

  int x = ctx.current.posX * 2;
  int y = ctx.current.posY;

  for (int i = 0; i < 4; ++i) {
    int row = getCurrentYPixelPos(i);
    int col = getCurrentXPixelPos(i);

    mvwprintw(gridWindow, y + row + 1, x + col * 2 + 1, "%ls", square);
    // mvwaddch(gridWindow, y + row + 1, x + col * 2 + 2, BLOCK);
  }

  wattroff(gridWindow, COLOR_PAIR(ctx.current.tetromino->color));
}

void printTetrominoPreviewOnGrid(int height) {
  wattron(gridWindow, COLOR_PAIR(ctx.current.tetromino->color));

  int x = ctx.current.posX * 2;
  int y = ctx.current.posY;

  for (int i = 0; i < 4; ++i) {
    int row = getCurrentYPixelPos(i);
    int col = getCurrentXPixelPos(i);

    mvwprintw(gridWindow, height + y + row + 1, x + col * 2 + 1, "%ls",
              wireSquare);
    // mvwaddch(gridWindow, y + row + 1, x + col * 2 + 2, BLOCK);
  }

  wattroff(gridWindow, COLOR_PAIR(ctx.current.tetromino->color));
}

////////////////////////////////////////////////////////////////////////////////
//// Other

int getCurrentFinalHeight() {
  int height;
  for (height = 0; height < GRID_ROWS - ctx.current.posY; ++height) {
    for (int i = 0; i < 4; ++i) {
      int col = getCurrentXPixelPos(i) + ctx.current.posX;
      int row = getCurrentYPixelPos(i) + height + ctx.current.posY + 1;

      // if (row <= 4 || grid[col][row] != 0) {
      if (row >= GRID_ROWS || grid[row][col] != 0) {
        return height;
      }
    }
  }

  return 0;
}

void checkLines() {
  int linesAccumulator = 0;
  for (int row = GRID_ROWS; row > 0; --row) {
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

  switch (linesAccumulator) {
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

  for (int col = 0; col < GRID_COLS; ++col) {
    if (grid[3][col] != 0) {
      endwin();
      printf("\n\n%lli\n\n", ctx.score);
      exit(0);
    }
  }
}

void updateCurrentTetromino() {
  ctx.current.tetromino = ctx.future;
  ctx.future = selectTetromino();
  ctx.current.posX = GRID_COLS / 4 + 1;
  ctx.current.posY = 0;
  ctx.current.rotation = 0;
}

void stickTetrominoToGrid() {
  for (int i = 0; i < 4; ++i) {
    int col = getCurrentXPixelPos(i);
    int row = getCurrentYPixelPos(i);

    grid[row + ctx.current.posY][col + ctx.current.posX] =
        ctx.current.tetromino->color;
  }
}

void checkTetrominoCollision() {
  if (ctx.touchdown == DONE) {
    return;
  }

  int accumulator = 0;

  for (int i = 0; i < 4; ++i) {
    int col = getCurrentXPixelPos(i) + ctx.current.posX;
    int row = getCurrentYPixelPos(i) + ctx.current.posY + 1;

    if (row >= GRID_ROWS || grid[row][col] != 0) {
      ctx.touchdown = TRUE;
    } else {
      ++accumulator;
    }
  }

  if (accumulator >= 4) {
    ctx.touchdown = FALSE;
  }
}

////////////////////////////////////////////////////////////////////////////////
//// Current tetromino controls

void moveLeft() {
  for (int i = 0; i < 4; ++i) {
    int col = getCurrentXPixelPos(i) + ctx.current.posX;
    int row = getCurrentYPixelPos(i) + ctx.current.posY;
    if (col == 0 || grid[row][col - 1] != 0) {
      return;
    }
  }

  --ctx.current.posX;
}

void moveRight() {
  for (int i = 0; i < 4; ++i) {
    int col = getCurrentXPixelPos(i) + ctx.current.posX;
    int row = getCurrentYPixelPos(i) + ctx.current.posY;

    if (col == GRID_COLS - 1 || grid[row][col + 1] != 0) {
      return;
    }
  }

  ++ctx.current.posX;
}

void rotate() {
  for (int i = 0; i < 4; ++i) {
    int col = getRotatedXPixelPos(i) + ctx.current.posX;
    int row = getCurrentYPixelPos(i) + ctx.current.posY;

    if (col == GRID_COLS || col == -1 || grid[row][col] != 0) {
      return;
    }
  }

  ctx.current.rotation == 3 ? ctx.current.rotation = 0 : ++ctx.current.rotation;
};

void moveBottom() {
  ctx.current.posY += getCurrentFinalHeight();

  stickTetrominoToGrid();
  updateCurrentTetromino();
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
  case KEY_UP:
    rotate();
    break;
  case ' ':
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
    printf("Usage: %s <fall per sec>\n", argv[0]);
    return 1;
  }

  ctx.touchdown = FALSE;
  ctx.isKeyPressed = FALSE;
  ctx.future = selectTetromino();
  ctx.score = 0;

  srand(time(NULL));
  setlocale(LC_ALL, "");

  initGraphics();

  updateCurrentTetromino();

  int levelSpeed = atoi(argv[1]);

  struct timeval startTime, currentTime;
  long elapsedTime;
  gettimeofday(&startTime, NULL);

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
    printGrid();
    printPreview();

    if (ctx.score) {
      mvprintw(LINES / 2 - GRID_ROWS / 2 + 4, COLS / 2 + GRID_COLS + 4, "%lli",
               ctx.score);
    }

    printTetrominoPreviewOnGrid(getCurrentFinalHeight());
    printTetrominoOnGrid();

    checkTetrominoCollision();
    checkLines();

    wrefresh(gridWindow);
    wrefresh(previewWindow);
  }

  return EXIT_SUCCESS;
}
