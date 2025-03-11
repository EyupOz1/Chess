#include "Utils.hpp"
#include "raylib.h"
#include "Defines.hpp"

Rectangle getPieceTexCoords(uint8_t piece)
{
    bool isBlack = (piece & 128) >> 7;
    uint8_t noColorPiece = piece & 0b01111111;

    Rectangle res = {0, 0, TEXTURE_PIECE_SIZE, TEXTURE_PIECE_SIZE};
    res.y = !isBlack * TEXTURE_PIECE_SIZE;
    switch (noColorPiece)
    {
    case Queen:
        res.x = 0 * TEXTURE_PIECE_SIZE;
        break;
    case King:
        res.x = 1 * TEXTURE_PIECE_SIZE;
        break;
    case Rook:
        res.x = 2 * TEXTURE_PIECE_SIZE;
        break;
    case Knight:
        res.x = 3 * TEXTURE_PIECE_SIZE;
        break;
    case Bishop:
        res.x = 4 * TEXTURE_PIECE_SIZE;
        break;
    case Pawn:
        res.x = 5 * TEXTURE_PIECE_SIZE;
        break;
    default:
        return {-1};
        break;
    }

    return res;
}

uint8_t fenToPiece(char fenChar)
{
    bool isWhite = 'A' <= fenChar && fenChar <= 'Z';
    char pieceInWhite = isWhite ? fenChar : fenChar - 32;

    uint8_t res = None;
    switch (pieceInWhite)
    {
    case 'K':
        res = King;
        break;
    case 'Q':
        res = Queen;
        break;
    case 'R':
        res = Rook;
        break;
    case 'N':
        res = Knight;
        break;
    case 'B':
        res = Bishop;
        break;
    case 'P':
        res = Pawn;
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