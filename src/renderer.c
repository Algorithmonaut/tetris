
#include "definitions.h"
#include <ncurses.h>
#include <wchar.h>

#ifdef USE_NERD_FONT
wchar_t *square = L"󰝤";
wchar_t *wireSquare = L"󰝣";
#endif /* ifdef USE_NERD_FONT */

#ifndef USE_NERD_FONT
wchar_t *square = L"▩";
wchar_t *wireSquare = L"▢";
#endif /* ifdef USE_NERD_FONT */

////

void renderGrid() {
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

void renderScore() { mvwprintw(scoreWindow, 1, 2, "%lli", ctx.score); }

void renderPreview() {
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      mvwaddch(previewWindow, row + 1, col * 2 + 1, ' ');
    }
  }

  for (int i = 0; i < 4; ++i) {
    int row = getFutureYPixelPos(i);
    int col = getFutureXPixelPos(i);
    wattron(previewWindow, COLOR_PAIR(ctx.future->color));
    mvwprintw(previewWindow, row + 1, col * 2 + 1, "%ls", square);
    wattroff(previewWindow, COLOR_PAIR(ctx.future->color));
  }
}

void renderTetromino() {
  wattron(gridWindow, COLOR_PAIR(ctx.current.tetromino->color));

  for (int i = 0; i < 4; ++i) {
    Vec2i pixelPos = getCurrentPixelPos(i);

    mvwprintw(gridWindow, ctx.current.posY + pixelPos.row + 1,
              (ctx.current.posX + pixelPos.col) * 2 + 1, "%ls", square);
  }

  wattroff(gridWindow, COLOR_PAIR(ctx.current.tetromino->color));
}

void renderTetrominoPreview() {
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

void renderAll() {
  renderGrid();
  renderPreview();
  renderScore();

  wrefresh(gridWindow);
  wrefresh(previewWindow);
  wrefresh(scoreWindow);
}
