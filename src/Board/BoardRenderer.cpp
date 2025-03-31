#include "Board/BoardRenderer.hpp"
#include "Utils/Utils.hpp"
#include "Core/SETTING.hpp"
#include "raylib.h"
#include "raymath.h"
#include <array>

void BoardRenderer::setup(Texture2D pieceTexture, Color cellLight, Color cellDark)
{
    this->pieceTexture = pieceTexture;
    this->cellLight = cellLight;
    this->cellDark = cellDark;

    // Board Cells
    for (int i = 0; i < 64; i++)
    {
        Vector2 cell = indexToCoords(i);
        Rectangle rec = {
            static_cast<float>(cell.x * CELL_SIZE),
            static_cast<float>(cell.y * CELL_SIZE),
            static_cast<float>(CELL_SIZE),
            static_cast<float>(CELL_SIZE)};
        this->cellDimensions[i] = rec;
    }
}

void BoardRenderer::drawBoard()
{
    for (int i = 0; i < 64; i++)
    {
        Vector2 cellCoord = indexToCoords(i);
        DrawRectangleRec(
            this->cellDimensions[i],
            static_cast<int>(cellCoord.x + cellCoord.y) % 2 == 0 ? RAYWHITE : BROWN);
    }
}

void BoardRenderer::drawPieces(std::array<uint8_t, 64> pieces)
{
    for (int i = 0; i < 64; i++)
    {
        if (pieces[i] == 0)
            continue;

        Vector2 cellCoord = indexToCoords(i);
        Vector2 cellPosW = Vector2Scale(cellCoord, CELL_SIZE); // World Position

        Rectangle src = pieceTextureCoords(pieces[i]);
        Rectangle dest = {cellPosW.x, cellPosW.y, CELL_SIZE, CELL_SIZE};
        DrawTexturePro(this->pieceTexture, src, dest, {0}, 0, WHITE);
    }
}

void BoardRenderer::highlightCell(int cellIndex)
{
    if (!isBetween(cellIndex, 0, 63))
        return;

    Vector2 pos = indexToCoords(cellIndex);
    DrawRectangle(pos.x * CELL_SIZE, pos.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, {0, 121, 241, 125});
}

void BoardRenderer::highlightCell(Vector2 targetWorldPos)
{
    Vector2 targetCellPos = this->getCellAtPosition(targetWorldPos);
    if (targetCellPos.x == -1)
    {
        return;
    }

    DrawRectangle(targetCellPos.x, targetCellPos.y, CELL_SIZE, CELL_SIZE, {0, 121, 241, 125});
}

Vector2 BoardRenderer::getCellAtPosition(Vector2 pos)
{
    for (int i = 0; i < 64; i++)
    {
        Rectangle currRect = this->cellDimensions[i];
        if (CheckCollisionPointRec(pos, currRect))
        {
            return {currRect.x, currRect.y};
        }
    }

    return {-1};
}
