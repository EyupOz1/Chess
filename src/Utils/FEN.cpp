#include "Utils/Utils.hpp"
#include <stdint.h>
#include <array>
#include <string>
#include "Core/SETTING.hpp"
#include "Board/BoardState.hpp"


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

