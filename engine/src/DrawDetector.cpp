#include "chess/DrawDetector.hpp"
#include "chess/Utils.hpp"

namespace Engine
{

    bool DrawDetector::IsInsufficientMaterial(const Position& pos) const
    {
        // Count all pieces except kings
        int whitePieces = 0, blackPieces = 0;
        int whiteMinors = 0, blackMinors = 0;  // Knights and bishops only

        for (int i = 0; i < Position::kSquareCount; ++i)
        {
            char piece = pos.PieceAt(i);
            if (piece == 0)
                continue;

            if (piece == 'K')
                continue;
            if (piece == 'k')
                continue;

            if (is_upper(piece))
            {
                whitePieces++;
                if (piece == 'N' || piece == 'B')
                    whiteMinors++;
            }
            else
            {
                blackPieces++;
                if (piece == 'n' || piece == 'b')
                    blackMinors++;
            }
        }

        // King vs King
        if (whitePieces == 0 && blackPieces == 0)
            return true;

        // King + Knight vs King
        if (whitePieces == 1 && blackPieces == 0 && whiteMinors == 1)
            return true;
        if (whitePieces == 0 && blackPieces == 1 && blackMinors == 1)
            return true;

        // King + Bishop vs King
        if (whitePieces == 1 && blackPieces == 0 && whiteMinors == 1)
            return true;
        if (whitePieces == 0 && blackPieces == 1 && blackMinors == 1)
            return true;

        // King + Bishop vs King + Bishop (same color bishops could still be drawable)
        // For simplicity, we don't claim draw here (could add more sophisticated logic)

        return false;
    }

    bool DrawDetector::IsThreefoldRepetition(const Game& game) const
    {
        const auto& positions = game.GetPositionHistory();
        if (positions.size() < 9)  // At least 9 positions (3 full moves for threefold)
            return false;

        // Count occurrences of the current position
        const Position& current = game.GetCurrentPosition();
        uint64_t currentHash = current.GetZobristHash();

        int count = 0;
        for (const auto& pos : positions)
        {
            if (pos.GetZobristHash() == currentHash)
                count++;
            if (count >= 3)
                return true;
        }

        return false;
    }

    bool DrawDetector::IsFiftyMoveRule(const Position& pos) const
    {
        return pos.GetHalfMoveClock() >= 100;  // 50 moves = 100 half-moves
    }

    std::string DrawDetector::GetDrawReason(const Game& game) const
    {
        const Position& pos = game.GetCurrentPosition();

        if (IsInsufficientMaterial(pos))
            return "Insufficient material";

        if (IsThreefoldRepetition(game))
            return "Threefold repetition";

        if (IsFiftyMoveRule(pos))
            return "Fifty-move rule";

        return "Draw";
    }

    uint64_t DrawDetector::ComputeZobristHash(const Position& pos) const
    {
        // Simplified zobrist hash (not full implementation)
        // In production, use lookup tables for each piece/square combination
        uint64_t hash = 0;

        for (int i = 0; i < Position::kSquareCount; ++i)
        {
            char piece = pos.PieceAt(i);
            if (piece != 0)
            {
                // Simple hash: XOR piece value with square index
                hash ^= (static_cast<uint64_t>(piece) << (i % 64));
            }
        }

        if (!pos.IsWhiteToMove())
            hash ^= 0xAAAAAAAAAAAAAAAAULL;

        return hash;
    }

    int DrawDetector::CountMaterial(const Position& pos, char piece) const
    {
        int count = 0;
        for (int i = 0; i < Position::kSquareCount; ++i)
        {
            if (pos.PieceAt(i) == piece)
                count++;
        }
        return count;
    }

}
