#include "chess/Game.hpp"
#include "chess/Rules.hpp"

namespace Engine
{

    Game::Game()
    {
        LoadStartPosition();
    }

    void Game::LoadStartPosition()
    {
        Clear();
        Position startPos;
        startPos.LoadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        positions_.push_back(startPos);
        currentMoveIndex_ = -1;
        statusDirty_ = true;
    }

    bool Game::LoadFromFEN(const std::string& fen)
    {
        Position pos;
        if (!pos.LoadFromFEN(fen))
            return false;

        Clear();
        positions_.push_back(pos);
        currentMoveIndex_ = -1;
        statusDirty_ = true;
        return true;
    }

    std::string Game::ExportFEN() const
    {
        return GetCurrentPosition().ToFEN();
    }

    const Position& Game::GetCurrentPosition() const
    {
        if (currentMoveIndex_ < 0 || currentMoveIndex_ >= static_cast<int>(positions_.size()))
            return positions_.back();
        return positions_[currentMoveIndex_ + 1];
    }

    Position& Game::GetCurrentPositionMutable()
    {
        if (currentMoveIndex_ < 0 || currentMoveIndex_ >= static_cast<int>(positions_.size()))
            return positions_.back();
        return positions_[currentMoveIndex_ + 1];
    }

    MoveResult Game::TryMove(int fromSquare, int toSquare, char promotion)
    {
        const Position& currentPos = GetCurrentPosition();
        char piece = currentPos.PieceAt(fromSquare);

        if (piece == 0)
            return {MoveError::NoPiece, {}};

        bool isWhite = is_upper(piece);
        if (isWhite != currentPos.IsWhiteToMove())
            return {MoveError::NotYourTurn, {}};

        // TODO: For now, create a Board from Position for compatibility
        // This will be refactored once Rules is fully updated
        Board board;
        for (int i = 0; i < 64; ++i)
            board.SetPiece(i, currentPos.PieceAt(i));
        board.SetTurn(currentPos.IsWhiteToMove());
        board.SetCastlingRights(currentPos.GetCastlingRights());
        board.SetEnPassantIndex(currentPos.GetEnPassantSquare());

        // Get legal moves and find the one matching our move
        auto legalMoves = Rules::GetLegalMovesForPiece(board, fromSquare);
        for (const Move& legalMove : legalMoves)
        {
            if (legalMove.to == toSquare)
            {
                if (legalMove.IsPromotion() && promotion == 0)
                    return {MoveError::PromotionRequired, legalMove};
                if (legalMove.IsPromotion())
                {
                    Move promMove = legalMove;
                    promMove.promotion = promotion;
                    return ApplyMove(promMove);
                }
                return ApplyMove(legalMove);
            }
        }

        return {MoveError::IllegalMove, {}};
    }

    MoveResult Game::ApplyMove(const Move& move)
    {
        // Truncate any redo history
        if (currentMoveIndex_ + 1 < static_cast<int>(positions_.size()))
        {
            positions_.erase(positions_.begin() + currentMoveIndex_ + 2, positions_.end());
            moves_.erase(moves_.begin() + currentMoveIndex_ + 1, moves_.end());
        }

        Position newPos = GetCurrentPosition();

        // Apply the move
        newPos.SetPiece(move.from, 0);
        if (move.type == MoveType::EnPassant)
        {
            int captureSquare = indexFromFileRank(
                fileFromIndex(move.to, Position::kBoardSize),
                rankFromIndex(move.from, Position::kBoardSize),
                Position::kBoardSize
            );
            newPos.SetPiece(captureSquare, 0);
        }
        else if (move.type == MoveType::Castle)
        {
            int file = fileFromIndex(move.to, Position::kBoardSize);
            if (file == 6)  // Kingside
            {
                int rookFrom = indexFromFileRank(7, rankFromIndex(move.to, Position::kBoardSize), Position::kBoardSize);
                int rookTo = indexFromFileRank(5, rankFromIndex(move.to, Position::kBoardSize), Position::kBoardSize);
                newPos.SetPiece(rookFrom, 0);
                newPos.SetPiece(rookTo, move.from == 4 ? 'R' : 'r');
            }
            else  // Queenside
            {
                int rookFrom = indexFromFileRank(0, rankFromIndex(move.to, Position::kBoardSize), Position::kBoardSize);
                int rookTo = indexFromFileRank(3, rankFromIndex(move.to, Position::kBoardSize), Position::kBoardSize);
                newPos.SetPiece(rookFrom, 0);
                newPos.SetPiece(rookTo, move.from == 4 ? 'R' : 'r');
            }
        }

        char movedPiece = move.pieceMoved ? move.pieceMoved : GetCurrentPosition().PieceAt(move.from);
        char promotionPiece = move.promotion ? move.promotion : movedPiece;
        newPos.SetPiece(move.to, promotionPiece);

        // Update castling rights
        std::array<bool, 4> newRights = newPos.GetCastlingRights();
        char movedPieceLower = to_lower(movedPiece);

        if (movedPieceLower == 'k')
        {
            bool isWhite = is_upper(movedPiece);
            if (isWhite)
                newRights[0] = newRights[1] = false;
            else
                newRights[2] = newRights[3] = false;
        }
        else if (movedPieceLower == 'r')
        {
            bool isWhite = is_upper(movedPiece);
            int fromFile = fileFromIndex(move.from, Position::kBoardSize);
            if (isWhite && fromFile == 0)
                newRights[1] = false;
            if (isWhite && fromFile == 7)
                newRights[0] = false;
            if (!isWhite && fromFile == 0)
                newRights[3] = false;
            if (!isWhite && fromFile == 7)
                newRights[2] = false;
        }

        // Check for captures affecting castling rights
        if (move.IsCapture())
        {
            bool isWhiteCapture = is_upper(move.pieceCaptured);
            int toFile = fileFromIndex(move.to, Position::kBoardSize);
            if (isWhiteCapture && toFile == 0)
                newRights[1] = false;
            if (isWhiteCapture && toFile == 7)
                newRights[0] = false;
            if (!isWhiteCapture && toFile == 0)
                newRights[3] = false;
            if (!isWhiteCapture && toFile == 7)
                newRights[2] = false;
        }

        newPos.SetCastlingRights(newRights);

        // Update en passant
        int epSquare = -1;
        if (movedPieceLower == 'p')
        {
            int fromRank = rankFromIndex(move.from, Position::kBoardSize);
            int toRank = rankFromIndex(move.to, Position::kBoardSize);
            if (std::abs(toRank - fromRank) == 2)
            {
                epSquare = indexFromFileRank(
                    fileFromIndex(move.from, Position::kBoardSize),
                    (fromRank + toRank) / 2,
                    Position::kBoardSize
                );
            }
        }
        newPos.SetEnPassantSquare(epSquare);

        // Update move counters
        int newHalfMoves = newPos.GetHalfMoveClock() + 1;
        if (move.IsCapture() || movedPieceLower == 'p')
            newHalfMoves = 0;

        int newFullMoves = newPos.GetFullMoveNumber();
        if (!newPos.IsWhiteToMove())
            newFullMoves++;

        newPos.SetWhiteToMove(!newPos.IsWhiteToMove());

        // Store the new position and move
        positions_.push_back(newPos);
        moves_.push_back(move);
        currentMoveIndex_++;
        statusDirty_ = true;

        return {MoveError::None, move};
    }

    void Game::Undo()
    {
        if (CanUndo())
        {
            currentMoveIndex_--;
            statusDirty_ = true;
        }
    }

    void Game::Redo()
    {
        if (CanRedo())
        {
            currentMoveIndex_++;
            statusDirty_ = true;
        }
    }

    bool Game::CanUndo() const
    {
        return currentMoveIndex_ >= 0;
    }

    bool Game::CanRedo() const
    {
        return currentMoveIndex_ + 1 < static_cast<int>(positions_.size()) - 1;
    }

    const std::vector<Move>& Game::GetMoveHistory() const
    {
        return moves_;
    }

    const std::vector<Position>& Game::GetPositionHistory() const
    {
        return positions_;
    }

    GameStatus Game::GetStatus() const
    {
        // Would call Rules::GetStatus(GetCurrentPosition())
        return lastComputedStatus_;
    }

    bool Game::IsGameOver() const
    {
        GameStatus status = GetStatus();
        return status == GameStatus::Checkmate ||
               status == GameStatus::Stalemate ||
               status == GameStatus::InsufficientMaterial ||
               status == GameStatus::ThreefoldRepetition ||
               status == GameStatus::FiftyMoveRule;
    }

    bool Game::IsInCheck() const
    {
        return GetStatus() == GameStatus::Check;
    }

    void Game::SetPlayerName(bool white, const std::string& name)
    {
        if (white)
            whitePlayerName_ = name;
        else
            blackPlayerName_ = name;
    }

    std::string Game::GetPlayerName(bool white) const
    {
        return white ? whitePlayerName_ : blackPlayerName_;
    }

    void Game::Clear()
    {
        positions_.clear();
        moves_.clear();
        currentMoveIndex_ = -1;
        statusDirty_ = true;
    }

    void Game::UpdateStatus()
    {
        // TODO: Implement using Rules and DrawDetector
        statusDirty_ = false;
    }

}
