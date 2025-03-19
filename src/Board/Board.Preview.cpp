#include "Board.hpp"
#include "Utils.hpp"
#include "algorithm"

std::vector<int> Board::getPossibleMoves(int index, int x)
{

    std::vector<int> pseudoLegalMoves;

    if (!isBetween(index, 0, 63))
        return pseudoLegalMoves;

    uint8_t currPiece = this->state[index];
    uint8_t pieceInWhite = getPieceInWhite(currPiece);
    Vector2 currPieceCoords = indexToCoords(index);

    if (pieceIsWhite(this->state[index]) == this->isBlacksTurn)
        return pseudoLegalMoves;

    switch (pieceInWhite)
    {
    case PAWN:
    {
        getPawnMoves(currPieceCoords, currPiece, pseudoLegalMoves);
        break;
    }
    case ROOK:
    {
        std::vector<Vector2> rookDirections = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
        getSlidingMoves(currPieceCoords, rookDirections, currPiece, pseudoLegalMoves);
        break;

        // TODO: Rochieren
    }
    case BISHOP:
    {
        std::vector<Vector2> bishopDirections = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        getSlidingMoves(currPieceCoords, bishopDirections, currPiece, pseudoLegalMoves);
        break;
    }
    case QUEEN:
    {
        std::vector<Vector2> queenDirections = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        getSlidingMoves(currPieceCoords, queenDirections, currPiece, pseudoLegalMoves);
        break;
    }
    case KNIGHT:
    {
        std::vector<Vector2> knightMoves = {
            {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};

        for (auto move : knightMoves)
        {
            Vector2 newCoord = {currPieceCoords.x + move.x, currPieceCoords.y + move.y};

            if (isBetween(newCoord.x, 0, 7) && isBetween(newCoord.y, 0, 7))
            {
                int newIndex = coordsToIndex(newCoord);

                if (this->state[newIndex] == NONE || isEnemyPiece(currPiece, this->state[newIndex]))
                {
                    pseudoLegalMoves.push_back(newIndex);
                }
            }
        }

        break;
    }

    case KING:
    {
        std::vector<Vector2> kingMoves = {
            {0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

        for (auto move : kingMoves)
        {
            Vector2 newCoord = {currPieceCoords.x + move.x, currPieceCoords.y + move.y};

            if (isBetween(newCoord.x, 0, 7) && isBetween(newCoord.y, 0, 7))
            {
                int newIndex = coordsToIndex(newCoord);

                if (this->state[newIndex] == NONE || isEnemyPiece(currPiece, this->state[newIndex]))
                {
                    pseudoLegalMoves.push_back(newIndex);
                }
            }
        }
    }
    }
    return pseudoLegalMoves;
}

void Board::getSlidingMoves(Vector2 coord, std::vector<Vector2> directions, int currPiece, std::vector<int> &possibleMoves)
{
    for (auto dir : directions)
    {
        Vector2 newCoord = coord;
        newCoord.x += dir.x;
        newCoord.y += dir.y;

        while (isBetween(newCoord.x, 0, 7) && isBetween(newCoord.y, 0, 7))
        {
            int newIndex = coordsToIndex(newCoord);
            if (this->state[newIndex] != NONE)
            {
                if (isEnemyPiece(currPiece, this->state[newIndex]))
                    possibleMoves.push_back(newIndex);
                break;
            }

            possibleMoves.push_back(newIndex);
            newCoord.x += dir.x;
            newCoord.y += dir.y;
        }
    }
}

void Board::getPawnMoves(Vector2 coord, int currPiece, std::vector<int> &possibleMoves)
{
    int direction = pieceIsWhite(currPiece) ? -1 : 1;
    int startRow = pieceIsWhite(currPiece) ? 6 : 1;

    Vector2 forward = {coord.x, coord.y + direction};
    int forwardIndex = coordsToIndex(forward);
    if (isBetween(forward.y, 0, 7) && this->state[forwardIndex] == NONE)
    {
        possibleMoves.push_back(forwardIndex);

        Vector2 doubleForward = {coord.x, coord.y + 2 * direction};
        int doubleForwardIndex = coordsToIndex(doubleForward);
        if (coord.y == startRow && this->state[doubleForwardIndex] == NONE)
        {
            possibleMoves.push_back(doubleForwardIndex);
        }
    }

    Vector2 captureLeft = {coord.x - 1, coord.y + direction};
    Vector2 captureRight = {coord.x + 1, coord.y + direction};

    for (Vector2 diag : {captureLeft, captureRight})
    {
        if (isBetween(diag.x, 0, 7) && isBetween(diag.y, 0, 7))
        {
            int diagIndex = coordsToIndex(diag);
            if (this->state[diagIndex] != NONE && isEnemyPiece(currPiece, this->state[diagIndex]))
            {
                possibleMoves.push_back(diagIndex);
            }
        }
    }

    // TODO: en passant - freue mich jz schon ...
}

/*
    std::vector<int> enemyPossibleMoves;
    if (x == 0)
    {

        for (size_t i = 0; i < 64; i++)
        {
            if (isEnemyPiece(this->state[index], this->state[i]))
            {
                std::vector<int> currEnemyMove = this->getPossibleMoves(this->state[i], 1);
                enemyPossibleMoves.insert(enemyPossibleMoves.end(), currEnemyMove.begin(), currEnemyMove.end());
            }
        }
    }

    /*
    if (std::find(enemyPossibleMoves.begin(), enemyPossibleMoves.end(), KING) != enemyPossibleMoves.end())
    {
        TraceLog(2, "King found");
    }
    */