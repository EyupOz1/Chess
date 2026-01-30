#pragma once

#include "chess/Board.hpp"
#include "chess/Rules.hpp"

#include <string>
#include <vector>

namespace GUI
{

    class GameController
    {
    public:
        GameController();

        void LoadStartPosition();
        bool LoadFen(const std::string &fen);
        std::string ExportFen() const;

        const Engine::Board &Board() const;
        Engine::Board &BoardMutable();

        Engine::MoveResult TryMove(int sourceIndex, int targetIndex, char promotion = 0);

        void UpdateCachedMoves(int selectedIndex, char selectedPiece, bool hasSelection);
        void ClearCachedMoves();
        const std::vector<Engine::Move> &CachedMoves() const;
        Engine::PositionStatus Status() const;

    private:
        void UpdateStatus();

        Engine::Board board_;
        std::vector<Engine::Move> cachedMoves_;
        Engine::PositionStatus status_ = Engine::PositionStatus::Ongoing;

        int lastSelectedIndex_ = -1;
        char lastSelectedPiece_ = 0;
        bool lastHasSelection_ = false;
    };

}
