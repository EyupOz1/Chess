#include "chess/Utils.hpp"

#include <cctype>

bool isChar(char character)
{
    return ('A' <= character && character <= 'Z') || ('a' <= character && character <= 'z');
}

bool is_lower(char c)
{
    return std::islower(static_cast<unsigned char>(c)) != 0;
}

bool is_upper(char c)
{
    return std::isupper(static_cast<unsigned char>(c)) != 0;
}

char to_lower(char c)
{
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

char to_upper(char c)
{
    return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

Vec2 indexToVec2(int index)
{
    Vec2 v;
    v.x = index % 8;
    v.y = index / 8;
    return v;
}

int vec2ToIndex(Vec2 v)
{
    return v.y * 8 + v.x;
}

int FileFromIndex(int index, int board_size)
{
    return index % board_size;
}

int RankFromIndex(int index, int board_size)
{
    return index / board_size;
}

int IndexFromFileRank(int file, int rank, int board_size)
{
    return rank * board_size + file;
}

bool IsOnBoard(int file, int rank, int board_size)
{
    return file >= 0 && file < board_size && rank >= 0 && rank < board_size;
}

bool IsFriendlyPiece(bool isWhiteTurn, char piece)
{
    if (piece == 0)
    {
        return false;
    }
    return isWhiteTurn ? is_upper(piece) : is_lower(piece);
}

bool IsOpponentPiece(bool isWhiteTurn, char piece)
{
    if (piece == 0)
    {
        return false;
    }
    return isWhiteTurn ? is_lower(piece) : is_upper(piece);
}
