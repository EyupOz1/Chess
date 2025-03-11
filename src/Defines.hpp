#pragma once

#include "stdint.h"

#define CELL_SIZE 60
#define TEXTURE_PIECE_SIZE 60

#define screenWidth 920
#define screenHeight 540

enum Piece : uint8_t
{
    None,
    Pawn,
    Bishop,
    Knight,
    Rook,
    Queen,
    King
};



enum MoveStatus
{

    Success,
    OutOfBounds

};
/*
First Bit -> Color

Color   White           Black
Pawn	1	00000001	129	10000001
Bishop	2	00000010	130	10000010
Knight	3	00000011	131	10000011
Rook	4	00000100	132	10000100
Queen	5	00000101	133	10000101
King	6	00000110	134	10000110
*/