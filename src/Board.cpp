#include "Board.hpp"
#include "raylib.h"
#include "raymath.h"
#include "Defines.hpp"
#include "Utils.hpp"
#include <iostream>
#include <string>
Board::Board() {}

void Board::setup(std::string fen, Texture2D tex)
{
    for (int i = 0; i < 64; i++)
    {
        this->state[i] = 0;

        // Setting up board cells for collision checks
        int x = i % 8, y = i / 8;
        Rectangle rec = {
            static_cast<float>(x * CELL_SIZE),
            static_cast<float>(y * CELL_SIZE),
            static_cast<float>(CELL_SIZE),
            static_cast<float>(CELL_SIZE)};
        this->boardCells[i] = rec;
    }

    // FEN
    int boardIndex = 0;
    for (int i = 0; i < fen.size(); i++)
    {
        char curr = fen[i];

        uint8_t resultingPiece = 0;
        if ('A' <= curr && curr <= 'z')
        {
            resultingPiece = fenToPiece(curr);
            this->state[boardIndex++] = resultingPiece;
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

    return Success;
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

Vector2 Board::pointToCellPos(Vector2 pos)
{
    for (int i = 0; i < 64; i++)
    {
        Rectangle currRect = this->boardCells[i];
        if (CheckCollisionPointRec(pos, currRect))
        {
            return {currRect.x, currRect.y};
        }
    }

    return {-1};
}