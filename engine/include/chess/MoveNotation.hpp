#pragma once

#include "Move.hpp"
#include "Position.hpp"
#include <string>

namespace Engine
{

    /**
     * MoveNotation provides conversion between different move formats.
     * Supports UCI (e2e4), SAN (e4, Nf3, O-O), and descriptive formats.
     */
    class MoveNotation
    {
    public:
        // Convert move to UCI notation (e.g., "e2e4", "e7e8q")
        static std::string ToUCI(const Move& move);

        // Convert move to Standard Algebraic Notation (e.g., "e4", "Nf3", "O-O")
        // Requires position context for disambiguation
        static std::string ToSAN(const Move& move, const Position& pos);

        // Convert move to long algebraic (e.g., "e2-e4", "e7-e8=Q")
        static std::string ToLAN(const Move& move);

        // Convert move to descriptive format
        static std::string ToDescriptive(const Move& move);

        // Parse UCI notation back to move (requires position context for validation)
        static Move ParseUCI(const std::string& uci);

    private:
        static char GetPieceSymbol(char piece);
        static std::string SquareToAlgebraic(int square);
        static int AlgebraicToSquare(const std::string& alg);
    };

}
