#pragma once

#include "Game.hpp"
#include "Position.hpp"
#include <vector>
#include <cstdint>

namespace Engine
{

    /**
     * DrawDetector handles all draw condition detection:
     * - Threefold repetition
     * - Fifty-move rule
     * - Insufficient material
     * - Stalemate (handled by Rules)
     */
    class DrawDetector
    {
    public:
        bool IsInsufficientMaterial(const Position& pos) const;
        bool IsThreefoldRepetition(const Game& game) const;
        bool IsFiftyMoveRule(const Position& pos) const;

        std::string GetDrawReason(const Game& game) const;

    private:
        uint64_t ComputeZobristHash(const Position& pos) const;
        int CountMaterial(const Position& pos, char piece) const;
    };

}
