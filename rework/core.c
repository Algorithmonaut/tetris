#include "main.h"

Vec2i getPixelPos(int pixel, int rotation, Tetrimino *tetrimino) {
  int pixelValue = tetrimino->model[rotation][pixel];
  return (Vec2i){pixelValue / 4, pixelValue % 4};
}

Vec2i getCurrentPixelPos(int pixel) {
  return getPixelPos(pixel, ctx.current.rotation, ctx.current.tetrimino);
}

Vec2i getRotatedPixelPos(int pixel) {
  int rotation = ctx.current.rotation == 3 ? 0 : ctx.current.rotation + 1;
  return getPixelPos(pixel, rotation, ctx.current.tetrimino);
}

Vec2i getNextPixelPos(int pixel) { return getPixelPos(pixel, 0, ctx.next); }
