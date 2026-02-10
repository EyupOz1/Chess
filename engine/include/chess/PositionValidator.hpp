#pragma once

#include "Position.hpp"
#include <string>

namespace Engine
{

    /**
     * PositionValidator checks positions for rule violations and validity.
     * Used for FEN import validation and ensuring legal positions.
     */
    class PositionValidator
    {
    public:
        struct ValidationError
        {
            bool isValid = true;
            std::string message;
        };

        // Check if a position is valid
        static ValidationError ValidatePosition(const Position& pos);

        // Specific validation functions
        static bool HasExactlyOneKingPerSide(const Position& pos);
        static bool NoPawnsOnFirstOrLastRank(const Position& pos);
        static bool KingsNotAdjacent(const Position& pos);
        static int CountPieces(const Position& pos, char piece);
        static bool IsLegalPawnStructure(const Position& pos);
    };

}
