#pragma once


#include "raylib.h"
#include "stdint.h"
#include "string"

Rectangle getPieceTexCoords(uint8_t piece);
uint8_t fenToPiece(char fenChar);
void FEN(uint8_t* boardState, std::string fen);

bool pieceIsWhite(uint8_t piece);
uint8_t getPieceInWhite(uint16_t piece);

Vector2 indexToCoords(int index);
int coordsToIndex(Vector2 vec);

/// @brief Basically the name, lower and upper is inclusive
/// @param lower 
/// @param upper 
/// @return 
bool isBetween(int input, int lower, int upper);

bool isEnemyPiece(uint8_t piece1, uint8_t piece2);