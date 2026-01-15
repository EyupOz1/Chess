#include "Chess/Board.hpp"
#include "Chess/Utils.hpp"
#include <iostream>

int Board::loadFen(std::string FEN)
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

    this->enPassantTarget = (FEN[i] == '-') ? "/" : FEN.substr(i, 2);

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

void Board::printBoard()
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

void Board::printInfo()
{
    std::string castlingRightsString;
    castlingRightsString.reserve(4);

    for (bool b : this->castlingRights)
        castlingRightsString.push_back(b ? '1' : '0');

    std::cout << "isWhiteTurn: " << this->isWhiteTurn
              << ", Castling Rights (KQkq): " << castlingRightsString
              << ", enPassantTarget: " << this->enPassantTarget
              << ", half-/fullMoves: " << this->halfMoves
              << "/" << this->fullMoves << "\n";

    this->printBoard();
}

int Board::forceMove(int sourceIndex, int targetIndex)
{

    char source = this->state[sourceIndex];

    if (source == 0)
    {
        std::cout << "no piece at index " << sourceIndex << "\n";
        return 1;
    }

    this->state[targetIndex] = source;
    this->state[sourceIndex] = 0;

    return 0;
}
