#pragma once


#include "raylib.h"
#include "stdint.h"

Rectangle getPieceTexCoords(uint8_t piece);
uint8_t fenToPiece(char fenChar);

bool pieceIsWhite(uint8_t piece);
uint8_t getPieceInWhite(uint16_t piece);
