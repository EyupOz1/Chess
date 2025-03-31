#pragma once

#include <array>
#include <stdint.h>
#include <string>

/// @brief Has a seperate Setup function
class BoardState
{
    std::array<uint8_t, 64> board;

    void FEN(std::string fen);

public:
    void setup(std::string fenString);
    void movePiece(int startIndex, int targetIndex);

    std::array<uint8_t, 64> getBoard();
};
