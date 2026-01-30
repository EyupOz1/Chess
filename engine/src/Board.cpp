#include "chess/Board.hpp"
#include "chess/Rules.hpp"
#include "chess/Utils.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace Engine
{
    namespace
    {
        bool IsFenPiece(char piece)
        {
            switch (piece)
            {
            case 'p':
            case 'r':
            case 'n':
            case 'b':
            case 'q':
            case 'k':
            case 'P':
            case 'R':
            case 'N':
            case 'B':
            case 'Q':
            case 'K':
                return true;
            default:
                return false;
            }
        }
    }

    Board::Board()
    {
        Clear();
    }

    char Board::PieceAt(int index) const
    {
        if (!IsValidIndex(index))
        {
            return 0;
        }
        return this->state_[index];
    }

    void Board::SetPiece(int index, char piece)
    {
        if (!IsValidIndex(index))
        {
            return;
        }
        this->state_[index] = piece;
    }

    bool Board::IsWhiteTurn() const
    {
        return this->isWhiteTurn_;
    }

    void Board::SetTurn(bool isWhiteTurn)
    {
        this->isWhiteTurn_ = isWhiteTurn;
    }

    const std::array<bool, 4> &Board::CastlingRights() const
    {
        return this->castlingRights_;
    }

    void Board::SetCastlingRights(const std::array<bool, 4> &rights)
    {
        this->castlingRights_ = rights;
    }

    const std::vector<Move> &Board::History() const
    {
        return this->history_;
    }

    std::vector<Move> &Board::History()
    {
        return this->history_;
    }

    int Board::HalfMoves() const
    {
        return this->halfMoves_;
    }

    int Board::FullMoves() const
    {
        return this->fullMoves_;
    }

    int Board::EnPassantIndex() const
    {
        return this->enPassantIndex_;
    }

    void Board::SetEnPassantIndex(int index)
    {
        this->enPassantIndex_ = IsValidIndex(index) ? index : -1;
    }

    bool Board::IsValidIndex(int index) const
    {
        return index >= 0 && index < kSquareCount;
    }

    void Board::Clear()
    {
        this->state_.fill(0);
        this->history_.clear();
        this->castlingRights_ = {};
        this->isWhiteTurn_ = true;
        this->halfMoves_ = 0;
        this->fullMoves_ = 1;
        this->enPassantIndex_ = -1;
    }

    int Board::LoadFen(std::string FEN)
    {
        this->state_.fill(0);
        this->history_.clear();
        this->enPassantIndex_ = -1;

        std::istringstream iss(FEN);
        std::string boardPart;
        std::string turnPart;
        std::string castlingPart;
        std::string enPassantPart;
        int halfMoves = 0;
        int fullMoves = 1;

        if (!(iss >> boardPart >> turnPart >> castlingPart >> enPassantPart >> halfMoves >> fullMoves))
        {
            return -1;
        }

        int rank = kBoardSize - 1;
        int file = 0;
        for (char curr : boardPart)
        {
            if (curr == '/')
            {
                if (file != kBoardSize)
                {
                    return -1;
                }
                rank--;
                file = 0;
                continue;
            }

            if ('1' <= curr && curr <= '8')
            {
                file += curr - '0';
                if (file > kBoardSize)
                {
                    return -1;
                }
                continue;
            }

            if (IsFenPiece(curr))
            {
                if (rank < 0 || file >= kBoardSize)
                {
                    return -1;
                }
                int index = indexFromFileRank(file, rank, kBoardSize);
                this->state_[index] = curr;
                file++;
                continue;
            }

            return -1;
        }

        if (rank != 0 || file != kBoardSize)
        {
            return -1;
        }

        if (turnPart != "w" && turnPart != "b")
        {
            return -1;
        }
        this->isWhiteTurn_ = (turnPart == "w");

        this->castlingRights_ = {};
        if (castlingPart != "-")
        {
            this->castlingRights_[0] = castlingPart.contains('K');
            this->castlingRights_[1] = castlingPart.contains('Q');
            this->castlingRights_[2] = castlingPart.contains('k');
            this->castlingRights_[3] = castlingPart.contains('q');
        }

        if (enPassantPart != "-")
        {
            if (enPassantPart.length() != 2)
            {
                return -1;
            }
            int epFile = enPassantPart[0] - 'a';
            int epRank = enPassantPart[1] - '1';
            if (!isOnBoard(epFile, epRank, kBoardSize))
            {
                return -1;
            }
            this->enPassantIndex_ = indexFromFileRank(epFile, epRank, kBoardSize);
        }

        this->halfMoves_ = halfMoves;
        this->fullMoves_ = fullMoves;

        return 0;
    }

    std::string Board::ExportFen() const
    {
        std::string fen;
        fen.reserve(80);

        for (int rank = kBoardSize - 1; rank >= 0; --rank)
        {
            int emptyCount = 0;
            for (int file = 0; file < kBoardSize; ++file)
            {
                int index = indexFromFileRank(file, rank, kBoardSize);
                char piece = this->state_[index];
                if (piece == 0)
                {
                    emptyCount++;
                    continue;
                }

                if (emptyCount > 0)
                {
                    fen.push_back(static_cast<char>('0' + emptyCount));
                    emptyCount = 0;
                }
                fen.push_back(piece);
            }

            if (emptyCount > 0)
            {
                fen.push_back(static_cast<char>('0' + emptyCount));
            }

            if (rank > 0)
            {
                fen.push_back('/');
            }
        }

        fen.push_back(' ');
        fen.push_back(this->isWhiteTurn_ ? 'w' : 'b');
        fen.push_back(' ');

        bool anyRights = false;
        if (this->castlingRights_[0])
        {
            fen.push_back('K');
            anyRights = true;
        }
        if (this->castlingRights_[1])
        {
            fen.push_back('Q');
            anyRights = true;
        }
        if (this->castlingRights_[2])
        {
            fen.push_back('k');
            anyRights = true;
        }
        if (this->castlingRights_[3])
        {
            fen.push_back('q');
            anyRights = true;
        }
        if (!anyRights)
        {
            fen.push_back('-');
        }

        fen.push_back(' ');
        if (this->enPassantIndex_ >= 0)
        {
            int file = fileFromIndex(this->enPassantIndex_, kBoardSize);
            int rank = rankFromIndex(this->enPassantIndex_, kBoardSize);
            fen.push_back(static_cast<char>('a' + file));
            fen.push_back(static_cast<char>('1' + rank));
        }
        else
        {
            fen.push_back('-');
        }

        fen.push_back(' ');
        fen += std::to_string(this->halfMoves_);
        fen.push_back(' ');
        fen += std::to_string(this->fullMoves_);

        return fen;
    }

    void Board::PrintBoard()
    {
        for (int i = 63; i >= 0; --i)
        {
            if ((i + 1) % 8 == 0)
            {
                std::cout << "\n";
            }

            char curr = this->state_[i];
            char piece = curr ? curr : ' ';
            std::cout << piece;
        }

        std::cout << "\n";
    }

    void Board::PrintInfo()
    {
        std::string castlingRightsString;
        castlingRightsString.reserve(4);

        for (bool b : this->castlingRights_)
        {
            castlingRightsString.push_back(b ? '1' : '0');
        }

        std::cout << "isWhiteTurn: " << this->isWhiteTurn_
                  << ", Castling Rights (KQkq): " << castlingRightsString
                  << ", half-/fullMoves: " << this->halfMoves_
                  << "/" << this->fullMoves_ << "\n";

        this->PrintBoard();
    }

    MoveResult Board::TryMove(int sourceIndex, int targetIndex, char promotion)
    {
        MoveResult result = {};

        if (!IsValidIndex(sourceIndex) || !IsValidIndex(targetIndex))
        {
            result.error = MoveError::IllegalMove;
            return result;
        }

        char source = this->state_[sourceIndex];
        if (source == 0)
        {
            result.error = MoveError::NoPiece;
            return result;
        }

        bool isWhitePiece = is_upper(source);
        if ((this->isWhiteTurn_ && !isWhitePiece) || (!this->isWhiteTurn_ && isWhitePiece))
        {
            result.error = MoveError::NotYourTurn;
            return result;
        }

        std::vector<Engine::Move> possibleMoves = Rules::GetLegalMovesForPiece(*this, sourceIndex);

        const Move *matched = nullptr;
        const Move *promotionMatch = nullptr;
        bool hasPromotionChoice = false;
        for (const Engine::Move &move : possibleMoves)
        {
            if (move.end != targetIndex)
            {
                continue;
            }

            if (move.promotion == 0)
            {
                matched = &move;
                break;
            }

            hasPromotionChoice = true;
            if (promotion != 0)
            {
                char normalized = isWhitePiece ? to_upper(promotion) : to_lower(promotion);
                if (to_lower(move.promotion) == to_lower(normalized))
                {
                    promotionMatch = &move;
                    break;
                }
            }
            else
            {
                matched = &move;
            }
        }

        if (promotionMatch != nullptr)
        {
            matched = promotionMatch;
        }

        if (matched == nullptr)
        {
            result.error = hasPromotionChoice ? MoveError::PromotionRequired : MoveError::IllegalMove;
            return result;
        }

        if (matched->promotion != 0 && promotion == 0)
        {
            result.error = MoveError::PromotionRequired;
            return result;
        }

        Move applied = *matched;
        if (promotion != 0)
        {
            applied.promotion = isWhitePiece ? to_upper(promotion) : to_lower(promotion);
        }

        return this->ApplyMove(applied);
    }

    MoveResult Board::ApplyMove(const Move &move)
    {
        MoveResult result = {};
        Move applied = move;

        if (!IsValidIndex(move.start) || !IsValidIndex(move.end))
        {
            result.error = MoveError::IllegalMove;
            return result;
        }

        char source = this->state_[move.start];
        if (source == 0)
        {
            result.error = MoveError::IllegalMove;
            return result;
        }

        char targetPiece = this->state_[move.end];
        if (targetPiece != 0 && is_upper(source) == is_upper(targetPiece))
        {
            result.error = MoveError::IllegalMove;
            return result;
        }

        Vec2 sourceCoords = indexToVec2(move.start);
        Vec2 targetCoords = indexToVec2(move.end);
        bool isPawn = to_lower(source) == 'p';
        bool isWhitePiece = is_upper(source);

        bool isKing = to_lower(source) == 'k';
        if (isKing && std::abs(targetCoords.x - sourceCoords.x) == 2)
        {
            applied.type = MoveType::Castle;
            if (targetCoords.x > sourceCoords.x)
            {
                int rookStart = vec2ToIndex({7, sourceCoords.y});
                int rookEnd = vec2ToIndex({5, sourceCoords.y});
                this->state_[rookEnd] = this->state_[rookStart];
                this->state_[rookStart] = 0;
            }
            else
            {
                int rookStart = vec2ToIndex({0, sourceCoords.y});
                int rookEnd = vec2ToIndex({3, sourceCoords.y});
                this->state_[rookEnd] = this->state_[rookStart];
                this->state_[rookStart] = 0;
            }
        }

        char pieceToPlace = source;
        if (isPawn && (targetCoords.y == 0 || targetCoords.y == 7))
        {
            if (applied.promotion != 0)
            {
                pieceToPlace = applied.promotion;
                applied.type = MoveType::Promotion;
            }
        }

        if (applied.type == MoveType::EnPassant)
        {
            if (this->enPassantIndex_ != move.end)
            {
                result.error = MoveError::IllegalMove;
                return result;
            }

            int capturedPawnY = isWhitePiece ? (targetCoords.y - 1) : (targetCoords.y + 1);
            int capturedIndex = vec2ToIndex({targetCoords.x, capturedPawnY});
            char capturedPiece = this->state_[capturedIndex];
            char expectedPawn = isWhitePiece ? 'p' : 'P';
            if (capturedPiece != expectedPawn)
            {
                result.error = MoveError::IllegalMove;
                return result;
            }
            this->state_[capturedIndex] = 0;
            targetPiece = isWhitePiece ? 'p' : 'P';
        }

        this->state_[move.end] = pieceToPlace;
        this->state_[move.start] = 0;

        this->enPassantIndex_ = -1;
        if (isPawn && std::abs(targetCoords.y - sourceCoords.y) == 2)
        {
            int epY = (sourceCoords.y + targetCoords.y) / 2;
            int epIndex = vec2ToIndex({sourceCoords.x, epY});
            this->enPassantIndex_ = epIndex;
        }

        if (isKing)
        {
            if (is_upper(source))
            {
                this->castlingRights_[0] = false;
                this->castlingRights_[1] = false;
            }
            else
            {
                this->castlingRights_[2] = false;
                this->castlingRights_[3] = false;
            }
        }

        if (to_lower(source) == 'r')
        {
            if (sourceCoords.x == 0 && sourceCoords.y == 0)
                this->castlingRights_[1] = false;
            if (sourceCoords.x == 7 && sourceCoords.y == 0)
                this->castlingRights_[0] = false;
            if (sourceCoords.x == 0 && sourceCoords.y == 7)
                this->castlingRights_[3] = false;
            if (sourceCoords.x == 7 && sourceCoords.y == 7)
                this->castlingRights_[2] = false;
        }

        if (to_lower(targetPiece) == 'r')
        {
            if (targetCoords.x == 0 && targetCoords.y == 0)
                this->castlingRights_[1] = false;
            if (targetCoords.x == 7 && targetCoords.y == 0)
                this->castlingRights_[0] = false;
            if (targetCoords.x == 0 && targetCoords.y == 7)
                this->castlingRights_[3] = false;
            if (targetCoords.x == 7 && targetCoords.y == 7)
                this->castlingRights_[2] = false;
        }

        bool isCapture = targetPiece != 0 || applied.type == MoveType::EnPassant;
        if (isPawn || isCapture)
        {
            this->halfMoves_ = 0;
        }
        else
        {
            this->halfMoves_ += 1;
        }

        if (!this->isWhiteTurn_)
        {
            this->fullMoves_ += 1;
        }

        this->history_.push_back(applied);
        this->isWhiteTurn_ = !this->isWhiteTurn_;

        result.error = MoveError::None;
        result.move = applied;
        return result;
    }

}
