
#pragma once

#include "raylib.h"
#include "chess/Utils.hpp"
#include <vector>

namespace GUI
{

    struct Board
    {
        Vector2 origin;
        float tileSize;
        int boardSize;
        bool flipped;

        Board(float tileSize, Vector2 center, bool flipped);

        void DrawBoard();
        void HighlightCells(const std::vector<Engine::Move> &moves);
    };

}
