#pragma once

#include <string>
#include <array>
#include <vector>

#include "Move.hpp"
#include "Utils.hpp"

namespace Engine
{

    class Board
    {
    public:
        static constexpr int kBoardSize = 8;
        static constexpr int kSquareCount = kBoardSize * kBoardSize;

        Board();

        char PieceAt(int index) const;
        void SetPiece(int index, char piece);

        bool IsWhiteTurn() const;
        void SetTurn(bool isWhiteTurn);

        const std::array<bool, 4> &CastlingRights() const;
        void SetCastlingRights(const std::array<bool, 4> &rights);

        const std::vector<Move> &History() const;
        std::vector<Move> &History();

        int HalfMoves() const;
        int FullMoves() const;
        int EnPassantIndex() const;
        void SetEnPassantIndex(int index);

        MoveResult TryMove(int sourceIndex, int targetIndex, char promotion = 0);
        MoveResult ApplyMove(const Move &move);

        void Clear();

        int LoadFen(std::string FEN);
        std::string ExportFen();

        void PrintBoard();
        void PrintInfo();

        void PossibleMoves();

    private:
        bool IsValidIndex(int index) const;

        std::array<char, kSquareCount> state_{};
        bool isWhiteTurn_ = true;
        std::array<bool, 4> castlingRights_{};

        std::vector<Move> history_;

        int halfMoves_ = 0;
        int fullMoves_ = 1;
        int enPassantIndex_ = -1;
    };

}
