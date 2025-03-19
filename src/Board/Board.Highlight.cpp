#include "Board.hpp"
#include "../Utils.hpp"

void Board::highlightCell(int cellIndex)
{
    if (!isBetween(cellIndex, 0, 63))
        return;

    Vector2 pos = indexToCoords(cellIndex);
    DrawRectangle(pos.x * CELL_SIZE, pos.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, {0, 121, 241, 125});
}

void Board::highlightCell(Vector2 cellWorldPos)
{
    Vector2 targetCellPos = this->getCellPos(cellWorldPos);
    if (targetCellPos.x == -1)
    {
        return;
    }

    DrawRectangle(targetCellPos.x, targetCellPos.y, CELL_SIZE, CELL_SIZE, {0, 121, 241, 125});
}
