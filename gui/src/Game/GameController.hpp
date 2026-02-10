#pragma once

#include "chess/Board.hpp"
#include "chess/Game.hpp"
#include "chess/Rules.hpp"
#include "chess/MoveNotation.hpp"
#include "chess/DrawDetector.hpp"

#include <string>
#include <vector>

namespace GUI
{

    class GameController
    {
    public:
        GameController();

        // Game initialization and control
        void LoadStartPosition();
        bool LoadFen(const std::string &fen);
        std::string ExportFen() const;

        // Board access
        const Engine::Board &Board() const;
        Engine::Board &BoardMutable();

        // Move handling
        Engine::MoveResult TryMove(int sourceIndex, int targetIndex, char promotion = 0);
        
        // Move history and navigation
        void Undo();
        void Redo();
        void GoToMove(int moveIndex);
        bool CanUndo() const;
        bool CanRedo() const;
        
        // Move history display
        const std::vector<Engine::Move> &GetMoveHistory() const;
        std::string GetMoveNotation(size_t moveIndex, bool useSAN = true) const;
        int GetCurrentMoveIndex() const;

        // Position information
        void UpdateCachedMoves(int selectedIndex, char selectedPiece, bool hasSelection);
        void ClearCachedMoves();
        const std::vector<Engine::Move> &CachedMoves() const;
        
        // Game status
        Engine::PositionStatus Status() const;
        std::string GetStatusString() const;
        bool IsInCheck() const;
        bool IsCheckmate() const;
        bool IsStalemate() const;
        bool IsDraw() const;
        std::string GetDrawReason() const;
        
        // Position metadata
        bool IsWhiteToMove() const;
        int GetHalfMoveClock() const;
        int GetFullMoveNumber() const;
        bool CanWhiteCastleKingside() const;
        bool CanWhiteCastleQueenside() const;
        bool CanBlackCastleKingside() const;
        bool CanBlackCastleQueenside() const;
        int GetEnPassantSquare() const;
        
        // Player information
        void SetPlayerName(bool white, const std::string &name);
        std::string GetPlayerName(bool white) const;

    private:
        void UpdateStatus();

        Engine::Board board_;
        Engine::Game game_;
        Engine::DrawDetector drawDetector_;
        
        std::vector<Engine::Move> cachedMoves_;
        Engine::PositionStatus status_ = Engine::PositionStatus::Ongoing;

        int lastSelectedIndex_ = -1;
        char lastSelectedPiece_ = 0;
        bool lastHasSelection_ = false;
    };

}
