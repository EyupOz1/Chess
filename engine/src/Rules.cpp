#include "chess/Rules.hpp"
#include "chess/MoveGen.hpp"
#include "chess/Utils.hpp"

#include <array>

namespace Engine
{

    namespace
    {
        bool IsValidIndex(int index)
        {
            return index >= 0 && index < Board::kSquareCount;
        }

        bool IsPieceColor(char piece, bool white)
        {
            return white ? is_upper(piece) : is_lower(piece);
        }

        int FindKingIndex(const Board &board, bool whiteKing)
        {
            char king = whiteKing ? 'K' : 'k';
            for (int i = 0; i < Board::kSquareCount; ++i)
            {
                if (board.PieceAt(i) == king)
                {
                    return i;
                }
            }
            return -1;
        }

        bool IsCastlePathSafe(const Board &board, const Move &move, bool whiteKing)
        {
            if (Rules::IsInCheck(board, whiteKing))
            {
                return false;
            }

            Vec2 start = indexToVec2(move.from);
            Vec2 end = indexToVec2(move.to);

            int step = (end.x > start.x) ? 1 : -1;
            int squaresToCheck[2] = {start.x + step, start.x + 2 * step};

            for (int file : squaresToCheck)
            {
                int index = vec2ToIndex({file, start.y});
                if (Rules::IsSquareAttacked(board, index, !whiteKing))
                {
                    return false;
                }
            }

            return true;
        }
    }

    bool Rules::IsSquareAttacked(const Board &board, int index, bool byWhite)
    {
        if (!IsValidIndex(index))
        {
            return false;
        }

        Vec2 target = indexToVec2(index);

        int pawnDir = byWhite ? -1 : 1;
        Vec2 pawnAttackers[2] = {
            {target.x - 1, target.y + pawnDir},
            {target.x + 1, target.y + pawnDir},
        };

        char pawnChar = byWhite ? 'P' : 'p';
        for (const Vec2 &pos : pawnAttackers)
        {
            if (!isOnBoard(pos.x, pos.y, Board::kBoardSize))
            {
                continue;
            }
            if (board.PieceAt(vec2ToIndex(pos)) == pawnChar)
            {
                return true;
            }
        }

        Vec2 knightOffsets[8] = {
            {1, 2},
            {2, 1},
            {-1, 2},
            {-2, 1},
            {1, -2},
            {2, -1},
            {-1, -2},
            {-2, -1},
        };

        char knightChar = byWhite ? 'N' : 'n';
        for (const Vec2 &offset : knightOffsets)
        {
            Vec2 pos = {target.x + offset.x, target.y + offset.y};
            if (!isOnBoard(pos.x, pos.y, Board::kBoardSize))
            {
                continue;
            }
            if (board.PieceAt(vec2ToIndex(pos)) == knightChar)
            {
                return true;
            }
        }

        Vec2 kingOffsets[8] = {
            {1, 0},
            {-1, 0},
            {0, 1},
            {0, -1},
            {1, 1},
            {-1, 1},
            {1, -1},
            {-1, -1},
        };

        char kingChar = byWhite ? 'K' : 'k';
        for (const Vec2 &offset : kingOffsets)
        {
            Vec2 pos = {target.x + offset.x, target.y + offset.y};
            if (!isOnBoard(pos.x, pos.y, Board::kBoardSize))
            {
                continue;
            }
            if (board.PieceAt(vec2ToIndex(pos)) == kingChar)
            {
                return true;
            }
        }

        struct Ray
        {
            Vec2 dir;
            std::array<char, 2> pieces;
        };

        std::array<Ray, 8> rays = {
            Ray{{1, 0}, {'r', 'q'}},
            Ray{{-1, 0}, {'r', 'q'}},
            Ray{{0, 1}, {'r', 'q'}},
            Ray{{0, -1}, {'r', 'q'}},
            Ray{{1, 1}, {'b', 'q'}},
            Ray{{-1, 1}, {'b', 'q'}},
            Ray{{1, -1}, {'b', 'q'}},
            Ray{{-1, -1}, {'b', 'q'}},
        };

        for (const Ray &ray : rays)
        {
            Vec2 current = target;
            while (true)
            {
                current.x += ray.dir.x;
                current.y += ray.dir.y;

                if (!isOnBoard(current.x, current.y, Board::kBoardSize))
                {
                    break;
                }

                char piece = board.PieceAt(vec2ToIndex(current));
                if (piece == 0)
                {
                    continue;
                }

                if (IsPieceColor(piece, byWhite))
                {
                    char lower = to_lower(piece);
                    if (lower == ray.pieces[0] || lower == ray.pieces[1])
                    {
                        return true;
                    }
                }
                break;
            }
        }

        return false;
    }

    bool Rules::IsInCheck(const Board &board, bool whiteKing)
    {
        int kingIndex = FindKingIndex(board, whiteKing);
        if (kingIndex < 0)
        {
            return false;
        }
        return IsSquareAttacked(board, kingIndex, !whiteKing);
    }

    bool Rules::IsLegalMove(const Board &board, const Move &move)
    {
        if (!IsValidIndex(move.from) || !IsValidIndex(move.to))
        {
            return false;
        }

        char piece = board.PieceAt(move.from);
        if (piece == 0)
        {
            return false;
        }

        bool whitePiece = is_upper(piece);
        if (move.type == MoveType::Castle)
        {
            if (!IsCastlePathSafe(board, move, whitePiece))
            {
                return false;
            }
        }

        Board copy = board;
        MoveResult result = copy.ApplyMove(move);
        if (!result.Ok())
        {
            return false;
        }

        return !IsInCheck(copy, whitePiece);
    }

    std::vector<Move> Rules::GetLegalMovesForPiece(const Board &board, int index)
    {
        std::vector<Move> legalMoves;

        if (!IsValidIndex(index))
        {
            return legalMoves;
        }

        char piece = board.PieceAt(index);
        if (piece == 0 || IsPieceColor(piece, board.IsWhiteTurn()) == false)
        {
            return legalMoves;
        }

        static MoveGen moveGen;
        std::vector<Move> moves = moveGen.GetPseudoLegalMoves(board, index);

        for (const Move &move : moves)
        {
            if (IsLegalMove(board, move))
            {
                legalMoves.push_back(move);
            }
        }

        return legalMoves;
    }

    bool Rules::HasAnyLegalMove(const Board &board, bool forWhite)
    {
        static MoveGen moveGen;
        for (int i = 0; i < Board::kSquareCount; ++i)
        {
            char piece = board.PieceAt(i);
            if (piece == 0 || IsPieceColor(piece, forWhite) == false)
            {
                continue;
            }

            std::vector<Move> moves = moveGen.GetPseudoLegalMoves(board, i);
            for (const Move &move : moves)
            {
                if (IsLegalMove(board, move))
                {
                    return true;
                }
            }
        }

        return false;
    }

    PositionStatus Rules::GetStatus(const Board &board)
    {
        bool whiteToMove = board.IsWhiteTurn();
        bool inCheck = IsInCheck(board, whiteToMove);
        bool hasMove = HasAnyLegalMove(board, whiteToMove);

        if (inCheck && !hasMove)
        {
            return PositionStatus::Checkmate;
        }

        if (!inCheck && !hasMove)
        {
            return PositionStatus::Stalemate;
        }

        if (inCheck)
        {
            return PositionStatus::Check;
        }

        return PositionStatus::Ongoing;
    }

}
