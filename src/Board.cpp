#include "Board.hpp"
#include "raylib.h"
#include "raymath.h"
#include "Defines.hpp"
#include "Utils.hpp"
#include <iostream>
#include <string>

Board::Board(std::string fen)
{
    this->cellSize = CELL_SIZE;
    for (int i = 0; i < 64; i++)
    {
        this->state[i] = 0;
    }

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
}

void Board::DrawBoard()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Rectangle rec = {i * this->cellSize, j * this->cellSize, this->cellSize, this->cellSize};
            Color col = (i + j) % 2 == 0 ? RAYWHITE : BROWN;
            DrawRectangleRec(rec, col);
        }
    }
}

void Board::DrawPieces(Texture2D &tex)
{
    for (int i = 0; i < 64; i++)
    {
        if (state[i] == 0)
            continue;

        Vector2 currCell = {i % 8, i / 8};
        Vector2 currCellWorldPos = Vector2Scale(currCell, CELL_SIZE);

        Rectangle src = getPieceTexCoords(this->state[i]);
        Rectangle dest = {currCellWorldPos.x, currCellWorldPos.y, CELL_SIZE, CELL_SIZE};
        DrawTexturePro(tex, src, dest, {0}, 0, WHITE);
    }
}