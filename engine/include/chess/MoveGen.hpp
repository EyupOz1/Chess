#pragma once

#include "Board.hpp"
#include "Move.hpp"
#include "Utils.hpp"

#include <vector>

namespace Engine
{

    struct MoveGen
    {
        std::vector<Move> GetPseudoLegalMoves(const Board &board, int index);

        std::vector<Move> GetPawnMoves(const Board &board, int index);
        std::vector<Move> GetKnightMoves(const Board &board, int index);
        std::vector<Move> GetBishopMoves(const Board &board, int index);
        std::vector<Move> GetRookMoves(const Board &board, int index);
        std::vector<Move> GetQueenMoves(const Board &board, int index);
        std::vector<Move> GetKingMoves(const Board &board, int index);
    };

}
