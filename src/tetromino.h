#ifndef TETROMINO_H
#define TETROMINO_H

#include "definitions.h"

extern Tetromino *tetrominos[TETROMINOS_COUNT];

#include "definitions.h"

void moveLeft();
void moveRight();
void rotate();
void moveBottom();

void nextCurrentTetromino();

#endif
