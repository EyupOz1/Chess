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
