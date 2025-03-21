#include "Utils/Utils.hpp"
#include "raylib.h"
#include <stdint.h>
#include "Core/SETTING.hpp"



Rectangle pieceTextureCoords(uint8_t piece)
{
    bool isBlack = !pieceIsWhite(piece);
    uint8_t pieceInWhite = getPieceInWhite(piece);

    Rectangle res = {0, 0, PIECE_TEXTURE_SIZE, PIECE_TEXTURE_SIZE};
    res.y = !isBlack * PIECE_TEXTURE_SIZE;
    switch (pieceInWhite)
    {
    case QUEEN:
        res.x = 0 * PIECE_TEXTURE_SIZE;
        break;
    case KING:
        res.x = 1 * PIECE_TEXTURE_SIZE;
        break;
    case ROOK:
        res.x = 2 * PIECE_TEXTURE_SIZE;
        break;
    case KNIGHT:
        res.x = 3 * PIECE_TEXTURE_SIZE;
        break;
    case BISHOP:
        res.x = 4 * PIECE_TEXTURE_SIZE;
        break;
    case PAWN:
        res.x = 5 * PIECE_TEXTURE_SIZE;
        break;
    default:
        return {-1};
        break;
    }

    return res;
}

