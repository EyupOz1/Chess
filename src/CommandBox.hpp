#pragma once

#include "raylib.h"
#include "Board/Board.hpp"

class CommandBox
{
public:
    char text[256] = "\0";
    bool isVisible = false;
    void update(Board& board);
};
