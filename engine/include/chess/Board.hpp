#pragma once

#include <string>
#include <array>
#include <vector>

#include "Utils.hpp"

namespace Engine
{

    struct Board
    {
        std::array<char, 64> state{};
        bool isWhiteTurn = true;
        std::array<bool, 4> castlingRights{};

        std::vector<Move> history;

        int halfMoves = 0;
        int fullMoves = 1;

        int Move(int sourceIndex, int targetIndex);

        int LoadFen(std::string FEN);
        std::string ExportFen();

        void PrintBoard();
        void PrintInfo();

        void PossibleMoves();
    };

}