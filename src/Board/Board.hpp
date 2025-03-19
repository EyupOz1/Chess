#pragma once

#include "raylib.h"
#include "stdint.h"
#include <string>
#include "../Defines.hpp"
#include "vector"

class Board
{
public:
    bool isBlacksTurn = false;

    uint8_t state[64];
    Rectangle boardCells[64];
    Texture2D tex;

    Board();

    void setup(std::string fen, Texture2D tex);
    MoveStatus move(int src, int dest);

    void drawBoard();
    void drawPieces();

    // Highlight
    void highlightCell(int cellIndex);
    void highlightCell(Vector2 cellWorldPos);

    // Preview
    std::vector<int> getPossibleMoves(int index, int x = 0);
    void getSlidingMoves(Vector2 coord, std::vector<Vector2> directions, int currPiece, std::vector<int> &possibleMoves);
    void getPawnMoves(Vector2 coord, int currPiece, std::vector<int> &possibleMoves);
    
    
    /// @brief 
    /// @param worldPos
    /// @return The world position of the Cell, {-1, -1} on error
    Vector2 getCellPos(Vector2 worldPos);

    int getCellIndex(Vector2 cellWorldPos);
};