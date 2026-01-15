
#pragma once

#include "raylib.h"

struct BoardView
{
    Vector2 origin;
    float tile_size;
    int board_size;
    bool flipped;
};

BoardView CreateBoardView(float tile_size, Vector2 center, bool flipped);
void DrawChessBoard(const BoardView &view);
