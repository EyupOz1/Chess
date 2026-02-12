#pragma once

#include "Position.hpp"
#include "Move.hpp"
#include "Utils.hpp"
#include "Board.hpp"
#include <vector>
#include <array>
#include <cstdint>

namespace Engine
{

    /**
     * MoveGenerator is responsible for legal and pseudo-legal move generation.
     * Uses pre-computed lookup tables for better performance.
     */
    class MoveGenerator
    {
    public:
        MoveGenerator();

        // Main API
        std::vector<Move> GetLegalMoves(const Position& pos);
        std::vector<Move> GetLegalMoves(const Position& pos, int fromSquare);
        std::vector<Move> GetPseudoLegalMoves(const Position& pos);
        std::vector<Move> GetPseudoLegalMoves(const Position& pos, int fromSquare);

        // Piece-specific move generation
        std::vector<Move> GetPawnMoves(const Position& pos, int fromSquare);
        std::vector<Move> GetKnightMoves(const Position& pos, int fromSquare);
        std::vector<Move> GetBishopMoves(const Position& pos, int fromSquare);
        std::vector<Move> GetRookMoves(const Position& pos, int fromSquare);
        std::vector<Move> GetQueenMoves(const Position& pos, int fromSquare);
        std::vector<Move> GetKingMoves(const Position& pos, int fromSquare);

        // Attack detection
        bool IsSquareAttacked(const Position& pos, int square, bool byWhite) const;
        uint64_t GetAttackedSquares(const Position& pos, bool byWhite) const;

    private:
        // Lookup tables (pre-computed once, reused)
        std::array<uint64_t, 64> knightAttacks_;
        std::array<uint64_t, 64> kingAttacks_;

        // Helpers
        uint64_t GetPawnAttackSquares(int square, bool isWhite) const;
        uint64_t GetRayAttacks(int square, const Position& pos, int dirX, int dirY) const;
        void AddMove(std::vector<Move>& moves, const Position& pos, int from, int to, 
                    MoveType type = MoveType::Normal, char promotion = 0) const;
        void AddPawnMove(std::vector<Move>& moves, const Position& pos, int from, int to) const;

        // Initialization
        void InitializeLookupTables();
        void InitializeKnightAttacks();
        void InitializeKingAttacks();
    };

    // Legacy compatibility: MoveGen is an alias for MoveGenerator, with Board support
    struct MoveGen
    {
        MoveGenerator generator;

        MoveGen() = default;

        std::vector<Move> GetPseudoLegalMoves(const Board &board, int index);
        std::vector<Move> GetPawnMoves(const Board &board, int index);
        std::vector<Move> GetKnightMoves(const Board &board, int index);
        std::vector<Move> GetBishopMoves(const Board &board, int index);
        std::vector<Move> GetRookMoves(const Board &board, int index);
        std::vector<Move> GetQueenMoves(const Board &board, int index);
        std::vector<Move> GetKingMoves(const Board &board, int index);
    };

}
