#include "raylib.h"
#include "stdint.h"
#include <string>


class Board
{
public:
    uint8_t state[64];


    char cellSize;
    Board(std::string fen);
    void DrawBoard();
    void DrawPieces(Texture2D &tex);
};