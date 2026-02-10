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
        game_.LoadStartPosition();
        UpdateStatus();
    }

    bool GameController::LoadFen(const std::string &fen)
    {
        int result = board_.LoadFen(fen);
        if (result != 0)
        {
            return false;
        }
        game_.LoadFromFEN(fen);
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
            // Also apply to game history
            game_.TryMove(sourceIndex, targetIndex, promotion);
            UpdateStatus();
        }
        return result;
    }

    void GameController::Undo()
    {
        if (CanUndo())
        {
            game_.Undo();
            // Rebuild board from game position
            const auto& pos = game_.GetCurrentPosition();
            board_.Clear();
            for (int i = 0; i < 64; ++i)
            {
                board_.SetPiece(i, pos.PieceAt(i));
            }
            board_.SetTurn(pos.IsWhiteToMove());
            board_.SetCastlingRights(pos.GetCastlingRights());
            board_.SetEnPassantIndex(pos.GetEnPassantSquare());
            ClearCachedMoves();
            UpdateStatus();
        }
    }

    void GameController::Redo()
    {
        if (CanRedo())
        {
            game_.Redo();
            // Rebuild board from game position
            const auto& pos = game_.GetCurrentPosition();
            board_.Clear();
            for (int i = 0; i < 64; ++i)
            {
                board_.SetPiece(i, pos.PieceAt(i));
            }
            board_.SetTurn(pos.IsWhiteToMove());
            board_.SetCastlingRights(pos.GetCastlingRights());
            board_.SetEnPassantIndex(pos.GetEnPassantSquare());
            ClearCachedMoves();
            UpdateStatus();
        }
    }

    void GameController::GoToMove(int moveIndex)
    {
        // Navigate to a specific move in history
        int currentIndex = game_.GetCurrentMoveIndex();
        while (currentIndex < moveIndex && CanRedo())
        {
            Redo();
            currentIndex++;
        }
        while (currentIndex > moveIndex && CanUndo())
        {
            Undo();
            currentIndex--;
        }
    }

    bool GameController::CanUndo() const
    {
        return game_.CanUndo();
    }

    bool GameController::CanRedo() const
    {
        return game_.CanRedo();
    }

    const std::vector<Engine::Move> &GameController::GetMoveHistory() const
    {
        return game_.GetMoveHistory();
    }

    std::string GameController::GetMoveNotation(size_t moveIndex, bool useSAN) const
    {
        if (moveIndex >= game_.GetMoveHistory().size())
            return "";
        
        const auto& move = game_.GetMoveHistory()[moveIndex];
        if (useSAN)
            return Engine::MoveNotation::ToSAN(move, game_.GetPositionHistory()[moveIndex]);
        else
            return Engine::MoveNotation::ToUCI(move);
    }

    int GameController::GetCurrentMoveIndex() const
    {
        return game_.GetCurrentMoveIndex();
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

    std::string GameController::GetStatusString() const
    {
        switch (status_)
        {
            case Engine::PositionStatus::Ongoing:
                return IsInCheck() ? "Check" : "Playing";
            case Engine::PositionStatus::Check:
                return "Check";
            case Engine::PositionStatus::Checkmate:
                return "Checkmate";
            case Engine::PositionStatus::Stalemate:
                return "Stalemate";
            default:
                return "Unknown";
        }
    }

    bool GameController::IsInCheck() const
    {
        return status_ == Engine::PositionStatus::Check;
    }

    bool GameController::IsCheckmate() const
    {
        return status_ == Engine::PositionStatus::Checkmate;
    }

    bool GameController::IsStalemate() const
    {
        return status_ == Engine::PositionStatus::Stalemate;
    }

    bool GameController::IsDraw() const
    {
        return drawDetector_.IsInsufficientMaterial(game_.GetCurrentPosition()) ||
               drawDetector_.IsThreefoldRepetition(game_) ||
               drawDetector_.IsFiftyMoveRule(game_.GetCurrentPosition());
    }

    std::string GameController::GetDrawReason() const
    {
        return drawDetector_.GetDrawReason(game_);
    }

    bool GameController::IsWhiteToMove() const
    {
        return board_.IsWhiteTurn();
    }

    int GameController::GetHalfMoveClock() const
    {
        return board_.HalfMoves();
    }

    int GameController::GetFullMoveNumber() const
    {
        return board_.FullMoves();
    }

    bool GameController::CanWhiteCastleKingside() const
    {
        return board_.CastlingRights()[0];
    }

    bool GameController::CanWhiteCastleQueenside() const
    {
        return board_.CastlingRights()[1];
    }

    bool GameController::CanBlackCastleKingside() const
    {
        return board_.CastlingRights()[2];
    }

    bool GameController::CanBlackCastleQueenside() const
    {
        return board_.CastlingRights()[3];
    }

    int GameController::GetEnPassantSquare() const
    {
        return board_.EnPassantIndex();
    }

    void GameController::SetPlayerName(bool white, const std::string &name)
    {
        game_.SetPlayerName(white, name);
    }

    std::string GameController::GetPlayerName(bool white) const
    {
        return game_.GetPlayerName(white);
    }

    void GameController::UpdateStatus()
    {
        status_ = Engine::Rules::GetStatus(board_);
    }

}
