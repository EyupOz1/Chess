#pragma once

#include "Board.hpp"
#include "Position.hpp"
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
        // Old API (using Board)
        static bool IsSquareAttacked(const Board &board, int index, bool byWhite);
        static bool IsInCheck(const Board &board, bool whiteKing);
        static bool IsLegalMove(const Board &board, const Move &move);
        static std::vector<Move> GetLegalMovesForPiece(const Board &board, int index);
        static bool HasAnyLegalMove(const Board &board, bool forWhite);
        static PositionStatus GetStatus(const Board &board);

        // New API (using Position)
        static bool IsSquareAttackedByPosition(const Position &pos, int square, bool byWhite);
        static bool IsInCheckByPosition(const Position &pos, bool whiteSide);
        static std::vector<Move> GetLegalMovesForPieceByPosition(const Position &pos, int square);
        static bool HasAnyLegalMoveByPosition(const Position &pos, bool forWhite);
        static PositionStatus GetStatusByPosition(const Position &pos);
    };

}
