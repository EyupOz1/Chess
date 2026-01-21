#include "Board.hpp"
#include "Utils.hpp"

#include <vector>

namespace Engine
{

    struct MoveGen
    {
        std::vector<Move> GetPseudoLegalMoves(Board &board, int index);

        std::vector<Move> GetPawnMoves(Board &board, int index);
        std::vector<Move> GetKnightMoves(Board &board, int index);
        std::vector<Move> GetBishopMoves(Board &board, int index);
        std::vector<Move> GetRookMoves(Board &board, int index);
        std::vector<Move> GetQueenMoves(Board &board, int index);
        std::vector<Move> GetKingMoves(Board &board, int index);
    };

}
