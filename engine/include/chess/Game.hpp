#pragma once

#include "Position.hpp"
#include "Move.hpp"
#include <vector>
#include <string>

namespace Engine
{

    enum class GameStatus
    {
        Ongoing,
        Check,
        Checkmate,
        Stalemate,
        InsufficientMaterial,
        ThreefoldRepetition,
        FiftyMoveRule
    };

    /**
     * Game manages the full game lifecycle, including move history and undo/redo.
     * This is separate from MoveGenerator and Rules which operate on Positions.
     */
    class Game
    {
    public:
        Game();

        // Initialization
        void LoadStartPosition();
        bool LoadFromFEN(const std::string& fen);
        std::string ExportFEN() const;

        // Position access
        const Position& GetCurrentPosition() const;
        Position& GetCurrentPositionMutable();

        // Move execution
        MoveResult TryMove(int fromSquare, int toSquare, char promotion = 0);
        MoveResult ApplyMove(const Move& move);

        // History navigation
        void Undo();
        void Redo();
        bool CanUndo() const;
        bool CanRedo() const;

        // History access
        const std::vector<Move>& GetMoveHistory() const;
        const std::vector<Position>& GetPositionHistory() const;
        int GetCurrentMoveIndex() const { return currentMoveIndex_; }

        // Game state
        GameStatus GetStatus() const;
        bool IsGameOver() const;
        bool IsInCheck() const;

        // Metadata
        void SetPlayerName(bool white, const std::string& name);
        std::string GetPlayerName(bool white) const;

        void Clear();

    private:
        std::vector<Position> positions_;      // Full position after each move
        std::vector<Move> moves_;               // Moves that led to each position
        int currentMoveIndex_ = -1;             // -1 = before first move, 0+ = after that move

        std::string whitePlayerName_ = "White";
        std::string blackPlayerName_ = "Black";

        GameStatus lastComputedStatus_ = GameStatus::Ongoing;
        bool statusDirty_ = true;

        void UpdateStatus();
    };

}
