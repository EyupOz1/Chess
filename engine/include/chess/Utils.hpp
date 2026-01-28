#pragma once

#include <string>
#include <exception>
#include <cctype>

#define ZERO_MOVE -1

namespace Engine
{

    bool isChar(char character);
    bool is_lower(char c);
    bool is_upper(char c);
    char to_lower(char c);
    char to_upper(char c);

    // Vec2
    struct Vec2
    {
        int x, y;
    };

    Vec2 indexToVec2(int index);
    int vec2ToIndex(Vec2 v);

    // Board

    int fileFromIndex(int index, int board_size);
    int rankFromIndex(int index, int board_size);
    int indexFromFileRank(int file, int rank, int board_size);
    bool isOnBoard(int file, int rank, int board_size);

    bool isFriendlyPiece(bool isWhiteTurn, char piece);
    bool isOpponentPiece(bool isWhiteTurn, char piece);

}
