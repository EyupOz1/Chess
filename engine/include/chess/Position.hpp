#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace Engine
{

    /**
     * Position represents an immutable snapshot of a chess position.
     * This is separate from Game which manages the history.
     */
    struct Position
    {
        static constexpr int kBoardSize = 8;
        static constexpr int kSquareCount = kBoardSize * kBoardSize;

        // Piece placement
        std::array<char, kSquareCount> pieces{};

        // Game state
        bool whiteToMove = true;
        std::array<bool, 4> castlingRights{};  // [wK, wQ, bK, bQ]
        int enPassantSquare = -1;

        // Move counters for draw detection
        int halfMoveClock = 0;  // Resets on pawn move or capture (fifty-move rule)
        int fullMoveNumber = 1;

        // Position hash for efficient repetition detection
        uint64_t zobristHash = 0;

        // Getters
        char PieceAt(int square) const;
        bool IsWhiteToMove() const { return whiteToMove; }
        const std::array<bool, 4>& GetCastlingRights() const { return castlingRights; }
        bool HasCastlingRight(int index) const { return castlingRights[index]; }
        int GetEnPassantSquare() const { return enPassantSquare; }
        int GetHalfMoveClock() const { return halfMoveClock; }
        int GetFullMoveNumber() const { return fullMoveNumber; }
        uint64_t GetZobristHash() const { return zobristHash; }

        // Setters
        void SetPiece(int square, char piece);
        void SetCastlingRights(const std::array<bool, 4>& rights) { castlingRights = rights; }
        void SetEnPassantSquare(int square) { enPassantSquare = square; }
        void SetWhiteToMove(bool white) { whiteToMove = white; }

        // Utilities
        bool IsValidSquare(int square) const { return square >= 0 && square < kSquareCount; }
        void Clear();

        // Import/Export
        bool LoadFromFEN(const std::string& fen);
        std::string ToFEN() const;

        // Comparison
        bool operator==(const Position& other) const;
        bool operator!=(const Position& other) const { return !(*this == other); }
    };

}
