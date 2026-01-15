#include "chess/MoveGen.hpp"
#include "chess/Utils.hpp"
#include <vector>

std::vector<int> MoveGen::getPseudoLegalMoves(Board &board, int index)
{

    std::vector<int> possibleMoves;

    std::vector<int> pawnMoves = this->getPawnMoves(board, index);
    possibleMoves.insert(possibleMoves.end(), pawnMoves.begin(), pawnMoves.end());

    

    return possibleMoves;
}

std::vector<int> MoveGen::getPawnMoves(Board &board, int index)
{

    std::vector<int> possibleMoves;

    if (is_upper(board.state[index]) && board.isWhiteTurn)
    {

        Vec2 coords = indexToVec2(index);

        Vec2 upCoords = {coords.x, coords.y + 1};
        int upIndex = vec2ToIndex(upCoords);

        Vec2 upUpCoords = {coords.x, coords.y + 2};
        int upUpIndex = vec2ToIndex(upUpCoords);

        Vec2 upRight = {coords.x + 1, coords.y};
        int upRightIndex = vec2ToIndex(upRight);

        Vec2 upLeft = {coords.x - 1, coords.y};
        int upLeftIndex = vec2ToIndex(upLeft);

        if (upRight.x < 7 && is_lower(board.state[upRightIndex]))
        {
            possibleMoves.push_back(upRightIndex);
        }

        if (upLeft.x > 0 && is_lower(board.state[upLeftIndex]))
        {
            possibleMoves.push_back(upLeftIndex);
        }

        if (board.state[upIndex] != 0)
            return possibleMoves;

        if (upCoords.y <= 6)
        {
            possibleMoves.push_back(upIndex);
        }

        if (coords.y == 1 && board.state[upUpIndex] == 0)
        {
            possibleMoves.push_back(upUpIndex);
        }
    
    }

    return possibleMoves;
}
