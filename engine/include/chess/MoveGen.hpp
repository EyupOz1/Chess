#include "Board.hpp"

#include <vector>

struct MoveGen
{
    std::vector<int> getPseudoLegalMoves(Board &board, int index);

    std::vector<int> getPawnMoves(Board &board, int index);
};
