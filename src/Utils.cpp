#include "raylib.h"
#include "string"
#include "stdint.h"

#include "Defines.hpp"
#include "Utils.hpp"


Rectangle getPieceTexCoords(uint8_t piece)
{
    bool isBlack = !pieceIsWhite(piece);
    uint8_t pieceInWhite = getPieceInWhite(piece);

    Rectangle res = {0, 0, TEXTURE_PIECE_SIZE, TEXTURE_PIECE_SIZE};
    res.y = !isBlack * TEXTURE_PIECE_SIZE;
    switch (pieceInWhite)
    {
    case QUEEN:
        res.x = 0 * TEXTURE_PIECE_SIZE;
        break;
    case KING:
        res.x = 1 * TEXTURE_PIECE_SIZE;
        break;
    case ROOK:
        res.x = 2 * TEXTURE_PIECE_SIZE;
        break;
    case KNIGHT:
        res.x = 3 * TEXTURE_PIECE_SIZE;
        break;
    case BISHOP:
        res.x = 4 * TEXTURE_PIECE_SIZE;
        break;
    case PAWN:
        res.x = 5 * TEXTURE_PIECE_SIZE;
        break;
    default:
        return {-1};
        break;
    }

    return res;
}


void FEN(uint8_t* boardState, std::string fen)
{
        int boardIndex = 0;
        for (int i = 0; i < fen.size(); i++)
        {
            char curr = fen[i];
    
            uint8_t resultingPiece = 0;
            if ('A' <= curr && curr <= 'z')
            {
                resultingPiece = fenToPiece(curr);
                boardState[boardIndex++] = resultingPiece;
                continue;
            }
    
            if (curr == '/')
            {
                while (boardIndex % 8 != 0)
                {
                    boardIndex++;
                }
                continue;
            }
    
            else if ('0' <= curr && curr <= '9')
            {
                boardIndex += (curr - 48);
            }
        }
}



uint8_t fenToPiece(char fenChar)
{
    bool isWhite = 'A' <= fenChar && fenChar <= 'Z';
    char pieceInWhite = isWhite ? fenChar : fenChar - 32;

    uint8_t res = NONE;
    switch (pieceInWhite)
    {
    case 'K':
        res = KING;
        break;
    case 'Q':
        res = QUEEN;
        break;
    case 'R':
        res = ROOK;
        break;
    case 'N':
        res = KNIGHT;
        break;
    case 'B':
        res = BISHOP;
        break;
    case 'P':
        res = PAWN;
        break;
    }

    if (!isWhite)
    {
        res |= 0b10000000;
    }

    return res;
}

bool pieceIsWhite(uint8_t piece)
{
    return !((piece & 128) >> 7);
}

uint8_t getPieceInWhite(uint16_t piece)
{
    if (!pieceIsWhite(piece))
    {
        piece &= 0b01111111;
    }
    return piece;
}

Vector2 indexToCoords(int index)
{
    return {index % 8, index / 8};
}

int coordsToIndex(Vector2 vec)
{
    return vec.x + vec.y * 8;
}


bool isBetween(int input, int lower, int upper)
{
    return (input >= lower && input <= upper);
    
}

bool isEnemyPiece(uint8_t piece1, uint8_t piece2)
{
return pieceIsWhite(piece1) != pieceIsWhite(piece2);
}