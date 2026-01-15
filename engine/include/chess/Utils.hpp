#pragma once

#include <string>
#include <exception>
#include <cctype>

bool isChar(char character);
bool is_lower(char c);
bool is_upper(char c);
char to_lower(char c);
char to_upper(char c);




// Vec2
struct Vec2
{
    int x, y;
};

Vec2 indexToVec2(int index);
int vec2ToIndex(Vec2 v);


// Board


int FileFromIndex(int index, int board_size);
int RankFromIndex(int index, int board_size);
int IndexFromFileRank(int file, int rank, int board_size);
bool IsOnBoard(int file, int rank, int board_size);

bool IsFriendlyPiece(bool isWhiteTurn, char piece);
bool IsOpponentPiece(bool isWhiteTurn, char piece);


struct Move {
    char piece;
    int start, end;
};
