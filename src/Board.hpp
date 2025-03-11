#pragma once


#include "raylib.h"
#include "stdint.h"
#include <string>
#include "Defines.hpp"

class Board
{
public:
    uint8_t state[64];
    Rectangle boardCells[64];
    Texture2D tex;

    Board();
    void setup(std::string fen, Texture2D tex);

    // 
    MoveStatus move(int src, int dest);


    // GFX
    void drawBoard();
    void drawPieces();
    Vector2 pointToCellPos(Vector2 pos);
};