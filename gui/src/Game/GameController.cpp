#include "GameController.hpp"

namespace GUI
{

    GameController::GameController()
    {
        LoadStartPosition();
    }

    void GameController::LoadStartPosition()
    {
        board_.LoadFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        UpdateStatus();
    }

    bool GameController::LoadFen(const std::string &fen)
    {
        int result = board_.LoadFen(fen);
        if (result != 0)
        {
            return false;
        }
        ClearCachedMoves();
        UpdateStatus();
        return true;
    }

    std::string GameController::ExportFen() const
    {
        return board_.ExportFen();
    }

    const Engine::Board &GameController::Board() const
    {
        return board_;
    }

    Engine::Board &GameController::BoardMutable()
    {
        return board_;
    }

    Engine::MoveResult GameController::TryMove(int sourceIndex, int targetIndex, char promotion)
    {
        Engine::MoveResult result = board_.TryMove(sourceIndex, targetIndex, promotion);
        if (result.Ok())
        {
            UpdateStatus();
        }
        return result;
    }

    void GameController::UpdateCachedMoves(int selectedIndex, char selectedPiece, bool hasSelection)
    {
        bool selectionChanged = !lastHasSelection_ ||
                                selectedIndex != lastSelectedIndex_ ||
                                selectedPiece != lastSelectedPiece_;

        if (hasSelection && selectionChanged)
        {
            cachedMoves_ = Engine::Rules::GetLegalMovesForPiece(board_, selectedIndex);
        }
        else if (!hasSelection)
        {
            cachedMoves_.clear();
        }

        lastHasSelection_ = hasSelection;
        lastSelectedIndex_ = selectedIndex;
        lastSelectedPiece_ = selectedPiece;
    }

    void GameController::ClearCachedMoves()
    {
        cachedMoves_.clear();
        lastHasSelection_ = false;
        lastSelectedIndex_ = -1;
        lastSelectedPiece_ = 0;
    }

    const std::vector<Engine::Move> &GameController::CachedMoves() const
    {
        return cachedMoves_;
    }

    Engine::PositionStatus GameController::Status() const
    {
        return status_;
    }

    void GameController::UpdateStatus()
    {
        status_ = Engine::Rules::GetStatus(board_);
    }

}
