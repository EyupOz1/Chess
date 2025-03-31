#include "Player.hpp"
#include "raylib.h"

void Player::handlePieceSelection(int targetIndex)
{
    if (this->selectedPiece == -1)
    {
        this->selectedPiece = targetIndex;
    }
    else if (this->selectedPiece == targetIndex)
    {
        this->selectedPiece = -1;
    }
    else if (this->selectedPiece != -1 &&
             std::find(selectedCellPossibleMoves.begin(), selectedCellPossibleMoves.end(), targetIndex) != selectedCellPossibleMoves.end())
    {
        board.move(selectedCell, targetIndex);
        board.switchTurn();
        selectedCell = -1;
        selectedCellPossibleMoves.clear();
    }
}