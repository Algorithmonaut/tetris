#include "core.h"
#include "main.h"
#include <ncurses.h>

#ifdef USE_NERD_FONT
wchar_t *square = L"󰝤";
wchar_t *wireSquare = L"󰝣";
#endif /* ifdef USE_NERD_FONT */

#ifndef USE_NERD_FONT
wchar_t *square = L"▩";
wchar_t *wireSquare = L"▢";
#endif /* ifdef USE_NERD_FONT */

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

void renderPreview() {
  // Clear the preview grid
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      mvwaddch(previewWindow, row + 1, col * 2 + 1, ' ');
    }
  }

  // Print the tetrimino
  for (int i = 0; i < 4; ++i) {
    Vec2i pixelPosition = getNextPixelPos(i);
    wattron(previewWindow, COLOR_PAIR(ctx.next->color));
    mvwprintw(previewWindow, pixelPosition.row + 1, pixelPosition.col * 2 + 1,
              "%ls", square);
    wattroff(previewWindow, COLOR_PAIR(ctx.next->color));
  }
}

void renderAll() {
  renderGrid();
  renderPreview();
  // renderScore();

  wrefresh(gridWindow);
  wrefresh(previewWindow);
  wrefresh(scoreWindow);
}
