#include <stdint.h>
#include "raylib.h"

bool pieceIsWhite(uint8_t piece)
{
    return !((piece & 128) >> 7);
}

uint8_t getPieceInWhite(uint16_t piece)
{
    if (!pieceIsWhite(piece))
    {
        piece &= 0b01111111;
    }
    return piece;
}

Vector2 indexToCoords(int index)
{
    return {index % 8, index / 8};
}

int coordsToIndex(Vector2 vec)
{
    return vec.x + vec.y * 8;
}

bool isBetween(int input, int lower, int upper)
{
    return (input >= lower && input <= upper);
}

bool isEnemyPiece(uint8_t piece1, uint8_t piece2)
{
    return pieceIsWhite(piece1) != pieceIsWhite(piece2);
}