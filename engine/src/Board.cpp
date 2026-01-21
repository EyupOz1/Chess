#include "Chess/Board.hpp"
#include "Chess/MoveGen.hpp"
#include "Chess/Utils.hpp"
#include <iostream>
#include <cstdlib>

namespace Engine
{

    int Board::LoadFen(std::string FEN)
    {
        std::cout << "Loading Fen -> " << FEN << "\n";

        int boardCursor = 63;
        size_t i = 0;
        for (; i < FEN.length(); i++)
        {
            char curr = FEN[i];

            if (curr == ' ')
            {
                i++;
                break;
            }

            else if (isChar(curr))
                this->state[boardCursor--] = curr;

            else if ('1' <= curr && curr <= '9')
            {
                boardCursor -= curr - 48;
            }

            else
                continue;
        }

        this->isWhiteTurn = FEN[i] == 'w';
        i += 2;

        int nextSpaceAfterCastling = FEN.find(' ', i);
        std::string castlingRights = FEN.substr(i, nextSpaceAfterCastling - i);

        this->castlingRights[0] = castlingRights.contains('K');
        this->castlingRights[1] = castlingRights.contains('Q');
        this->castlingRights[2] = castlingRights.contains('k');
        this->castlingRights[3] = castlingRights.contains('q');

        i = nextSpaceAfterCastling + 1;

        if (FEN[i] != '-')
        {
        }
        else
        {
        }
        // TODO: EN PASSANT out of FEN

        int halfMovesIndex = FEN.find(' ', i) + 1;
        int nextSpaceAfterHM = FEN.find(' ', halfMovesIndex);
        std::string halfMovesStr = FEN.substr(halfMovesIndex, nextSpaceAfterHM - halfMovesIndex);
        this->halfMoves = std::stoi(halfMovesStr);

        int fullMovesIndex = FEN.find(' ', halfMovesIndex) + 1;
        int nextSpaceAfterFM = FEN.find(' ', fullMovesIndex);

        std::string fullMovesStr = FEN.substr(fullMovesIndex, nextSpaceAfterFM - fullMovesIndex);
        this->fullMoves = std::stoi(fullMovesStr);

        // TODO: make function safer to rogue input

        return 0;
    }

    void Board::PrintBoard()
    {
        for (int i = 63; i >= 0; --i)
        {
            if ((i + 1) % 8 == 0)
                std::cout << "\n";

            char curr = this->state[i];
            char piece = curr ? curr : ' ';
            std::cout << piece;
        }

        std::cout << "\n";
    }

    void Board::PrintInfo()
    {
        std::string castlingRightsString;
        castlingRightsString.reserve(4);

        for (bool b : this->castlingRights)
            castlingRightsString.push_back(b ? '1' : '0');

        std::cout << "isWhiteTurn: " << this->isWhiteTurn
                  << ", Castling Rights (KQkq): " << castlingRightsString
                  //<< ", enPassantTarget: " << this->enPassantTarget
                  << ", half-/fullMoves: " << this->halfMoves
                  << "/" << this->fullMoves << "\n";

        this->PrintBoard();
    }

    int Board::Move(int sourceIndex, int targetIndex)
    {

        char source = this->state[sourceIndex];
        char targetPiece = this->state[targetIndex];

        if (source == 0)
        {
            std::cout << "no piece at index " << sourceIndex << "\n";
            return 1;
        }

        if ((this->isWhiteTurn && !is_upper(source)) || (!this->isWhiteTurn && !is_lower(source)))
        {
            std::cout << "not your turn\n";
            return 1;
        }

        MoveGen moveGen;
        std::vector<Engine::Move> possibleMoves = moveGen.GetPseudoLegalMoves(*this, sourceIndex);
        bool isLegal = false;
        for (const Engine::Move &move : possibleMoves)
        {
            if (move.end == targetIndex)
            {
                isLegal = true;
                break;
            }
        }

        if (!isLegal)
        {
            std::cout << "illegal move\n";
            return 1;
        }
        
        Engine::Move move = {.piece = this->state[sourceIndex], .start = sourceIndex, .end = targetIndex};

        Vec2 sourceCoords = indexToVec2(sourceIndex);
        Vec2 targetCoords = indexToVec2(targetIndex);
        bool isPawn = to_lower(source) == 'p';
        bool isDiagonal = std::abs(targetCoords.x - sourceCoords.x) == 1;
        bool isForward = is_upper(source) ? (targetCoords.y - sourceCoords.y == 1) : (sourceCoords.y - targetCoords.y == 1);
        bool isTargetEmpty = this->state[targetIndex] == 0;

        if (isPawn && isDiagonal && isForward && isTargetEmpty)
        {
            int capturedPawnY = is_upper(source) ? (targetCoords.y - 1) : (targetCoords.y + 1);
            int capturedIndex = vec2ToIndex({targetCoords.x, capturedPawnY});
            char captured = this->state[capturedIndex];
            bool isEnemyPawn = to_lower(captured) == 'p' &&
                               ((is_upper(source) && is_lower(captured)) ||
                                (is_lower(source) && is_upper(captured)));
            if (isEnemyPawn)
            {
                this->state[capturedIndex] = 0;
            }
        }

        bool isKing = to_lower(source) == 'k';
        if (isKing && std::abs(targetCoords.x - sourceCoords.x) == 2)
        {
            if (targetCoords.x > sourceCoords.x)
            {
                int rookStart = vec2ToIndex({7, sourceCoords.y});
                int rookEnd = vec2ToIndex({5, sourceCoords.y});
                this->state[rookEnd] = this->state[rookStart];
                this->state[rookStart] = 0;
            }
            else
            {
                int rookStart = vec2ToIndex({0, sourceCoords.y});
                int rookEnd = vec2ToIndex({3, sourceCoords.y});
                this->state[rookEnd] = this->state[rookStart];
                this->state[rookStart] = 0;
            }
        }

        this->state[targetIndex] = source;
        this->state[sourceIndex] = 0;

        if (isKing)
        {
            if (is_upper(source))
            {
                this->castlingRights[0] = false;
                this->castlingRights[1] = false;
            }
            else
            {
                this->castlingRights[2] = false;
                this->castlingRights[3] = false;
            }
        }

        if (to_lower(source) == 'r')
        {
            if (sourceCoords.x == 0 && sourceCoords.y == 0)
                this->castlingRights[1] = false;
            if (sourceCoords.x == 7 && sourceCoords.y == 0)
                this->castlingRights[0] = false;
            if (sourceCoords.x == 0 && sourceCoords.y == 7)
                this->castlingRights[3] = false;
            if (sourceCoords.x == 7 && sourceCoords.y == 7)
                this->castlingRights[2] = false;
        }

        if (to_lower(targetPiece) == 'r')
        {
            if (targetCoords.x == 0 && targetCoords.y == 0)
                this->castlingRights[1] = false;
            if (targetCoords.x == 7 && targetCoords.y == 0)
                this->castlingRights[0] = false;
            if (targetCoords.x == 0 && targetCoords.y == 7)
                this->castlingRights[3] = false;
            if (targetCoords.x == 7 && targetCoords.y == 7)
                this->castlingRights[2] = false;
        }


        this->history.push_back(move);
        this->isWhiteTurn = !this->isWhiteTurn;

        return 0;
    }

}
