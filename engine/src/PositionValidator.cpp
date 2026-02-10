#include "chess/PositionValidator.hpp"
#include "chess/Utils.hpp"

namespace Engine
{

    PositionValidator::ValidationError PositionValidator::ValidatePosition(const Position& pos)
    {
        // Check kings
        if (!HasExactlyOneKingPerSide(pos))
            return {false, "Each side must have exactly one king"};

        if (KingsNotAdjacent(pos))
            return {false, "Kings cannot be adjacent"};

        // Check pawns
        if (NoPawnsOnFirstOrLastRank(pos))
            return {false, "Pawns cannot be on first or last rank"};

        if (!IsLegalPawnStructure(pos))
            return {false, "Illegal pawn structure"};

        return {true, ""};
    }

    bool PositionValidator::HasExactlyOneKingPerSide(const Position& pos)
    {
        int whiteKings = 0, blackKings = 0;
        for (int i = 0; i < Position::kSquareCount; ++i)
        {
            char piece = pos.PieceAt(i);
            if (piece == 'K')
                whiteKings++;
            else if (piece == 'k')
                blackKings++;
        }
        return whiteKings == 1 && blackKings == 1;
    }

    bool PositionValidator::NoPawnsOnFirstOrLastRank(const Position& pos)
    {
        // Check first rank (rank 0)
        for (int file = 0; file < 8; ++file)
        {
            char piece = pos.PieceAt(indexFromFileRank(file, 0, 8));
            if (to_lower(piece) == 'p')
                return true;  // Invalid
        }

        // Check last rank (rank 7)
        for (int file = 0; file < 8; ++file)
        {
            char piece = pos.PieceAt(indexFromFileRank(file, 7, 8));
            if (to_lower(piece) == 'p')
                return true;  // Invalid
        }

        return false;  // Valid
    }

    bool PositionValidator::KingsNotAdjacent(const Position& pos)
    {
        int wKing = -1, bKing = -1;
        for (int i = 0; i < Position::kSquareCount; ++i)
        {
            if (pos.PieceAt(i) == 'K')
                wKing = i;
            else if (pos.PieceAt(i) == 'k')
                bKing = i;
        }

        if (wKing < 0 || bKing < 0)
            return false;

        int wFile = fileFromIndex(wKing, 8);
        int wRank = rankFromIndex(wKing, 8);
        int bFile = fileFromIndex(bKing, 8);
        int bRank = rankFromIndex(bKing, 8);

        return std::abs(wFile - bFile) <= 1 && std::abs(wRank - bRank) <= 1;
    }

    int PositionValidator::CountPieces(const Position& pos, char piece)
    {
        int count = 0;
        for (int i = 0; i < Position::kSquareCount; ++i)
        {
            if (pos.PieceAt(i) == piece)
                count++;
        }
        return count;
    }

    bool PositionValidator::IsLegalPawnStructure(const Position& pos)
    {
        // White pawns cannot have more than 8 (original + promotions)
        // Black pawns cannot have more than 8
        // In practice, maximum would be 8 originals
        // For simplicity, we just check they're not excessive

        int whitePawns = CountPieces(pos, 'P');
        int blackPawns = CountPieces(pos, 'p');

        // More than 8 of each type is impossible
        if (whitePawns > 8 || blackPawns > 8)
            return false;

        // Could add more advanced checks (e.g., doubled pawns, pawn on wrong files)
        // But basic validation is sufficient for now

        return true;
    }

}
