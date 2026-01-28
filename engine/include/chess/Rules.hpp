#pragma once

#include "Board.hpp"
#include "Move.hpp"

#include <vector>

namespace Engine
{

    enum class PositionStatus
    {
        Ongoing,
        Check,
        Checkmate,
        Stalemate
    };

    struct Rules
    {
        static bool IsSquareAttacked(const Board &board, int index, bool byWhite);
        static bool IsInCheck(const Board &board, bool whiteKing);
        static bool IsLegalMove(const Board &board, const Move &move);
        static std::vector<Move> GetLegalMovesForPiece(const Board &board, int index);
        static bool HasAnyLegalMove(const Board &board, bool forWhite);
        static PositionStatus GetStatus(const Board &board);
    };

}
