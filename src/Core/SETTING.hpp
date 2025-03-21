#pragma once

#include <stdint.h>

#define WINDOW_WIDTH 920
#define WINDOW_HEIGHT 540

#define FEN_startingPos "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
#define FEN_testPos "r1bk3r/p2pBpNp/n4n2/1p1NP2P/6P1/3P4/P1P1K3/"

#define CELL_SIZE 60
#define PIECE_TEXTURE_SIZE 60



enum Piece : uint8_t
{
    NONE,
    PAWN,
    BISHOP,
    KNIGHT,
    ROOK,
    QUEEN,
    KING
};

enum MoveStatus
{
    SUCCESS,
    OUTOFBOUNDS
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