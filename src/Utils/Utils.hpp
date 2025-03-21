#pragma once

#include <array>
#include <stdint.h>
#include <string>
#include "Board/BoardState.hpp"
#include "raylib.h"


uint8_t fenToPiece(char fenChar);

bool pieceIsWhite(uint8_t piece);
uint8_t getPieceInWhite(uint16_t piece);
Vector2 indexToCoords(int index);
int coordsToIndex(Vector2 vec);
bool isBetween(int input, int lower, int upper);
bool isEnemyPiece(uint8_t piece1, uint8_t piece2);

Rectangle pieceTextureCoords(uint8_t piece);