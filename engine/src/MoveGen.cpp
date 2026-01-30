#include "chess/MoveGen.hpp"
#include "chess/Utils.hpp"

#include <vector>

namespace Engine
{

    std::vector<Move> MoveGen::GetPseudoLegalMoves(const Board &board, int index)
    {
        std::vector<Move> possibleMoves;

        char piece = board.PieceAt(index);
        if (piece == 0)
            return possibleMoves;

        char lower = to_lower(piece);

        if (lower == 'p')
            return this->GetPawnMoves(board, index);

        if (lower == 'n')
            return this->GetKnightMoves(board, index);

        if (lower == 'b')
            return this->GetBishopMoves(board, index);

        if (lower == 'r')
            return this->GetRookMoves(board, index);

        if (lower == 'q')
            return this->GetQueenMoves(board, index);

        if (lower == 'k')
            return this->GetKingMoves(board, index);

        return possibleMoves;
    }

    static void AddPawnMove(std::vector<Move> &moves, char pawn, int from, int to)
    {
        Vec2 targetCoords = indexToVec2(to);
        bool isPromotionRank = is_upper(pawn) ? (targetCoords.y == 7) : (targetCoords.y == 0);
        if (!isPromotionRank)
        {
            moves.push_back({pawn, from, to});
            return;
        }

        char promoPieces[4] = {'q', 'r', 'b', 'n'};
        for (char promo : promoPieces)
        {
            char normalized = is_upper(pawn) ? to_upper(promo) : to_lower(promo);
            Move move = {pawn, from, to};
            move.promotion = normalized;
            move.type = MoveType::Promotion;
            moves.push_back(move);
        }
    }

    std::vector<Move> MoveGen::GetPawnMoves(const Board &board, int index)
    {
        std::vector<Move> possibleMoves;

        char pawn = board.PieceAt(index);
        if (pawn == 0)
            return possibleMoves;

        Vec2 coords = indexToVec2(index);
        bool isWhitePawn = is_upper(pawn);

        if (isWhitePawn)
        {
            Vec2 upCoords = {coords.x, coords.y + 1};
            int upIndex = vec2ToIndex(upCoords);

            Vec2 upUpCoords = {coords.x, coords.y + 2};
            int upUpIndex = vec2ToIndex(upUpCoords);

            Vec2 upRight = {coords.x + 1, coords.y + 1};
            int upRightIndex = vec2ToIndex(upRight);

            Vec2 upLeft = {coords.x - 1, coords.y + 1};
            int upLeftIndex = vec2ToIndex(upLeft);

            if (upRight.x <= 7 && is_lower(board.PieceAt(upRightIndex)))
            {
                AddPawnMove(possibleMoves, pawn, index, upRightIndex);
            }

            if (upLeft.x >= 0 && is_lower(board.PieceAt(upLeftIndex)))
            {
                AddPawnMove(possibleMoves, pawn, index, upLeftIndex);
            }

            if (upCoords.y <= 7 && board.PieceAt(upIndex) == 0)
            {
                AddPawnMove(possibleMoves, pawn, index, upIndex);

                if (coords.y == 1 && board.PieceAt(upUpIndex) == 0)
                {
                    possibleMoves.push_back({pawn, index, upUpIndex});
                }
            }

            int enPassantIndex = board.EnPassantIndex();
            if (enPassantIndex >= 0)
            {
                Vec2 enPassantCoords = indexToVec2(enPassantIndex);
                if (enPassantCoords.y == coords.y + 1 &&
                    (enPassantCoords.x == coords.x + 1 || enPassantCoords.x == coords.x - 1))
                {
                    if (board.PieceAt(enPassantIndex) == 0)
                    {
                        int capturedIndex = vec2ToIndex({enPassantCoords.x, coords.y});
                        if (board.PieceAt(capturedIndex) == 'p')
                        {
                            possibleMoves.push_back({pawn, index, enPassantIndex, 0, MoveType::EnPassant});
                        }
                    }
                }
            }
        }
        else
        {
            Vec2 downCoords = {coords.x, coords.y - 1};
            int downIndex = vec2ToIndex(downCoords);

            Vec2 downDownCoords = {coords.x, coords.y - 2};
            int downDownIndex = vec2ToIndex(downDownCoords);

            Vec2 downRight = {coords.x + 1, coords.y - 1};
            int downRightIndex = vec2ToIndex(downRight);

            Vec2 downLeft = {coords.x - 1, coords.y - 1};
            int downLeftIndex = vec2ToIndex(downLeft);

            if (downRight.x <= 7 && is_upper(board.PieceAt(downRightIndex)))
            {
                AddPawnMove(possibleMoves, pawn, index, downRightIndex);
            }

            if (downLeft.x >= 0 && is_upper(board.PieceAt(downLeftIndex)))
            {
                AddPawnMove(possibleMoves, pawn, index, downLeftIndex);
            }

            if (board.PieceAt(downIndex) != 0)
                return possibleMoves;

            if (downCoords.y >= 0)
            {
                AddPawnMove(possibleMoves, pawn, index, downIndex);
            }

            if (coords.y == 6 && board.PieceAt(downDownIndex) == 0)
            {
                possibleMoves.push_back({pawn, index, downDownIndex});
            }

            int enPassantIndex = board.EnPassantIndex();
            if (enPassantIndex >= 0)
            {
                Vec2 enPassantCoords = indexToVec2(enPassantIndex);
                if (enPassantCoords.y == coords.y - 1 &&
                    (enPassantCoords.x == coords.x + 1 || enPassantCoords.x == coords.x - 1))
                {
                    if (board.PieceAt(enPassantIndex) == 0)
                    {
                        int capturedIndex = vec2ToIndex({enPassantCoords.x, coords.y});
                        if (board.PieceAt(capturedIndex) == 'P')
                        {
                            possibleMoves.push_back({pawn, index, enPassantIndex, 0, MoveType::EnPassant});
                        }
                    }
                }
            }
        }

        return possibleMoves;
    }

    std::vector<Move> MoveGen::GetKnightMoves(const Board &board, int index)
    {
        std::vector<Move> possibleMoves;

        char piece = board.PieceAt(index);
        if (piece == 0)
            return possibleMoves;

        bool isWhitePiece = is_upper(piece);
        Vec2 coords = indexToVec2(index);

        Vec2 offsets[8] = {
            {1, 2},
            {2, 1},
            {-1, 2},
            {-2, 1},
            {1, -2},
            {2, -1},
            {-1, -2},
            {-2, -1},
        };

        for (Vec2 offset : offsets)
        {
            Vec2 target = {coords.x + offset.x, coords.y + offset.y};
            if (target.x < 0 || target.x > 7 || target.y < 0 || target.y > 7)
                continue;

            int targetIndex = vec2ToIndex(target);
            char targetPiece = board.PieceAt(targetIndex);

            if (targetPiece == 0 ||
                (isWhitePiece && is_lower(targetPiece)) ||
                (!isWhitePiece && is_upper(targetPiece)))
            {
                possibleMoves.push_back({piece, index, targetIndex});
            }
        }

        return possibleMoves;
    }

    std::vector<Move> MoveGen::GetBishopMoves(const Board &board, int index)
    {
        std::vector<Move> possibleMoves;

        char piece = board.PieceAt(index);
        if (piece == 0)
            return possibleMoves;

        bool isWhitePiece = is_upper(piece);
        Vec2 coords = indexToVec2(index);

        Vec2 directions[4] = {
            {1, 1},
            {-1, 1},
            {1, -1},
            {-1, -1},
        };

        for (Vec2 dir : directions)
        {
            Vec2 current = coords;
            while (true)
            {
                current.x += dir.x;
                current.y += dir.y;

                if (current.x < 0 || current.x > 7 || current.y < 0 || current.y > 7)
                    break;

                int targetIndex = vec2ToIndex(current);
                char targetPiece = board.PieceAt(targetIndex);

                if (targetPiece == 0)
                {
                    possibleMoves.push_back({piece, index, targetIndex});
                    continue;
                }

                if ((isWhitePiece && is_lower(targetPiece)) ||
                    (!isWhitePiece && is_upper(targetPiece)))
                {
                    possibleMoves.push_back({piece, index, targetIndex});
                }

                break;
            }
        }

        return possibleMoves;
    }

    std::vector<Move> MoveGen::GetRookMoves(const Board &board, int index)
    {
        std::vector<Move> possibleMoves;

        char piece = board.PieceAt(index);
        if (piece == 0)
            return possibleMoves;

        bool isWhitePiece = is_upper(piece);
        Vec2 coords = indexToVec2(index);

        Vec2 directions[4] = {
            {1, 0},
            {-1, 0},
            {0, 1},
            {0, -1},
        };

        for (Vec2 dir : directions)
        {
            Vec2 current = coords;
            while (true)
            {
                current.x += dir.x;
                current.y += dir.y;

                if (current.x < 0 || current.x > 7 || current.y < 0 || current.y > 7)
                    break;

                int targetIndex = vec2ToIndex(current);
                char targetPiece = board.PieceAt(targetIndex);

                if (targetPiece == 0)
                {
                    possibleMoves.push_back({piece, index, targetIndex});
                    continue;
                }

                if ((isWhitePiece && is_lower(targetPiece)) ||
                    (!isWhitePiece && is_upper(targetPiece)))
                {
                    possibleMoves.push_back({piece, index, targetIndex});
                }

                break;
            }
        }

        return possibleMoves;
    }

    std::vector<Move> MoveGen::GetQueenMoves(const Board &board, int index)
    {
        std::vector<Move> possibleMoves;

        char piece = board.PieceAt(index);
        if (piece == 0)
            return possibleMoves;

        bool isWhitePiece = is_upper(piece);
        Vec2 coords = indexToVec2(index);

        Vec2 directions[8] = {
            {1, 0},
            {-1, 0},
            {0, 1},
            {0, -1},
            {1, 1},
            {-1, 1},
            {1, -1},
            {-1, -1},
        };

        for (Vec2 dir : directions)
        {
            Vec2 current = coords;
            while (true)
            {
                current.x += dir.x;
                current.y += dir.y;

                if (current.x < 0 || current.x > 7 || current.y < 0 || current.y > 7)
                    break;

                int targetIndex = vec2ToIndex(current);
                char targetPiece = board.PieceAt(targetIndex);

                if (targetPiece == 0)
                {
                    possibleMoves.push_back({piece, index, targetIndex});
                    continue;
                }

                if ((isWhitePiece && is_lower(targetPiece)) ||
                    (!isWhitePiece && is_upper(targetPiece)))
                {
                    possibleMoves.push_back({piece, index, targetIndex});
                }

                break;
            }
        }

        return possibleMoves;
    }

    std::vector<Move> MoveGen::GetKingMoves(const Board &board, int index)
    {
        std::vector<Move> possibleMoves;

        char piece = board.PieceAt(index);
        if (piece == 0)
            return possibleMoves;

        bool isWhitePiece = is_upper(piece);
        Vec2 coords = indexToVec2(index);

        Vec2 offsets[8] = {
            {1, 0},
            {-1, 0},
            {0, 1},
            {0, -1},
            {1, 1},
            {-1, 1},
            {1, -1},
            {-1, -1},
        };

        for (Vec2 offset : offsets)
        {
            Vec2 target = {coords.x + offset.x, coords.y + offset.y};
            if (target.x < 0 || target.x > 7 || target.y < 0 || target.y > 7)
                continue;

            int targetIndex = vec2ToIndex(target);
            char targetPiece = board.PieceAt(targetIndex);

            if (targetPiece == 0 ||
                (isWhitePiece && is_lower(targetPiece)) ||
                (!isWhitePiece && is_upper(targetPiece)))
            {
                possibleMoves.push_back({piece, index, targetIndex});
            }
        }

        if (isWhitePiece && coords.x == 4 && coords.y == 0)
        {
            const auto &rights = board.CastlingRights();
            if (rights[0])
            {
                int f1 = vec2ToIndex({5, 0});
                int g1 = vec2ToIndex({6, 0});
                int h1 = vec2ToIndex({7, 0});
                if (board.PieceAt(f1) == 0 && board.PieceAt(g1) == 0)
                {
                    if (board.PieceAt(h1) == 'R')
                    {
                        possibleMoves.push_back({piece, index, g1, 0, MoveType::Castle});
                    }
                }
            }

            if (rights[1])
            {
                int b1 = vec2ToIndex({1, 0});
                int c1 = vec2ToIndex({2, 0});
                int d1 = vec2ToIndex({3, 0});
                int a1 = vec2ToIndex({0, 0});
                if (board.PieceAt(b1) == 0 && board.PieceAt(c1) == 0 && board.PieceAt(d1) == 0)
                {
                    if (board.PieceAt(a1) == 'R')
                    {
                        possibleMoves.push_back({piece, index, c1, 0, MoveType::Castle});
                    }
                }
            }
        }

        if (!isWhitePiece && coords.x == 4 && coords.y == 7)
        {
            const auto &rights = board.CastlingRights();
            if (rights[2])
            {
                int f8 = vec2ToIndex({5, 7});
                int g8 = vec2ToIndex({6, 7});
                int h8 = vec2ToIndex({7, 7});
                if (board.PieceAt(f8) == 0 && board.PieceAt(g8) == 0)
                {
                    if (board.PieceAt(h8) == 'r')
                    {
                        possibleMoves.push_back({piece, index, g8, 0, MoveType::Castle});
                    }
                }
            }

            if (rights[3])
            {
                int b8 = vec2ToIndex({1, 7});
                int c8 = vec2ToIndex({2, 7});
                int d8 = vec2ToIndex({3, 7});
                int a8 = vec2ToIndex({0, 7});
                if (board.PieceAt(b8) == 0 && board.PieceAt(c8) == 0 && board.PieceAt(d8) == 0)
                {
                    if (board.PieceAt(a8) == 'r')
                    {
                        possibleMoves.push_back({piece, index, c8, 0, MoveType::Castle});
                    }
                }
            }
        }

        return possibleMoves;
    }

}
