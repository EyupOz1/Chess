#include "Board.hpp"
#include "raylib.h"
#include "raymath.h"
#include "Defines.hpp"
#include "Utils.hpp"
#include <iostream>
#include <string>
#include "vector"

Board::Board() {}

void Board::setup(std::string fen, Texture2D tex)
{
    for (int i = 0; i < 64; i++)
    {
        this->state[i] = 0;

        // Setting up board cells for collision checks
        Vector2 cell = indexToCoords(i);
        Rectangle rec = {
            static_cast<float>(cell.x * CELL_SIZE),
            static_cast<float>(cell.y * CELL_SIZE),
            static_cast<float>(CELL_SIZE),
            static_cast<float>(CELL_SIZE)};
        this->boardCells[i] = rec;
    }

    FEN(this->state, fen);

    // Tex
    this->tex = tex;
}

MoveStatus Board::move(int src, int dest)
{

    uint8_t piece = getPieceInWhite(this->state[src]);

    if (this->state[src])
    {
        /* code */
    }

    uint8_t temp = this->state[src];
    this->state[src] = 0;

    this->state[dest] = temp;

    return SUCCESS;
}

void Board::drawBoard()
{
    for (int i = 0; i < 64; i++)
    {

        int x = i % 8, y = i / 8;

        DrawRectangleRec(this->boardCells[i], (x + y) % 2 == 0 ? RAYWHITE : BROWN);
    }
}

void Board::drawPieces()
{
    for (int i = 0; i < 64; i++)
    {
        if (state[i] == 0)
            continue;

        Vector2 currCell = {static_cast<float>(i % 8), static_cast<float>(i / 8)};
        Vector2 currCellWorldPos = Vector2Scale(currCell, CELL_SIZE);

        Rectangle src = getPieceTexCoords(this->state[i]);
        Rectangle dest = {currCellWorldPos.x, currCellWorldPos.y, CELL_SIZE, CELL_SIZE};
        DrawTexturePro(this->tex, src, dest, {0}, 0, WHITE);
    }
}

Vector2 Board::getCellPos(Vector2 worldPos)
{
    for (int i = 0; i < 64; i++)
    {
        Rectangle currRect = this->boardCells[i];
        if (CheckCollisionPointRec(worldPos, currRect))
        {
            return {currRect.x, currRect.y};
        }
    }

    return {-1};
}

int Board::getCellIndex(Vector2 cellWorldPos)
{
    Vector2 localCellPos = {cellWorldPos.x / CELL_SIZE, cellWorldPos.y / CELL_SIZE};

    return coordsToIndex(localCellPos);

}
