#include "chess/MoveGen.hpp"
#include "chess/Utils.hpp"
#include <vector>
#include <iostream>
namespace Engine
{

    std::vector<Move> MoveGen::GetPseudoLegalMoves(Board &board, int index)
    {

        std::vector<Move> possibleMoves;

        char piece = board.state[index];
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

    std::vector<Move> MoveGen::GetPawnMoves(Board &board, int index)
    {

        std::vector<Move> possibleMoves;

        if (is_upper(board.state[index]) && board.isWhiteTurn)
        {

            Vec2 coords = indexToVec2(index);

            Vec2 upCoords = {coords.x, coords.y + 1};
            int upIndex = vec2ToIndex(upCoords);

            Vec2 upUpCoords = {coords.x, coords.y + 2};
            int upUpIndex = vec2ToIndex(upUpCoords);

            Vec2 upRight = {coords.x + 1, coords.y + 1};
            int upRightIndex = vec2ToIndex(upRight);

            Vec2 upLeft = {coords.x - 1, coords.y + 1};
            int upLeftIndex = vec2ToIndex(upLeft);

            if (upRight.x <= 7 && is_lower(board.state[upRightIndex]))
            {
                possibleMoves.push_back({board.state[index], index, upRightIndex});
            }

            if (upLeft.x >= 0 && is_lower(board.state[upLeftIndex]))
            {
                possibleMoves.push_back({board.state[index], index, upLeftIndex});
            }

            if (upCoords.y <= 6 && board.state[upIndex] == 0)
            {
                possibleMoves.push_back({board.state[index], index, upIndex});

                if (coords.y == 1 && board.state[upUpIndex] == 0)
                {
                    possibleMoves.push_back({board.state[index], index, upUpIndex});
                }
            }

            if (!board.history.empty())
            {
                Move &lastMove = board.history.back();
                Vec2 lastMoveStartCoords = indexToVec2(lastMove.start);
                Vec2 lastMoveEndCoords = indexToVec2(lastMove.end);

                if (to_lower(lastMove.piece) == 'p' && lastMoveStartCoords.y == 6 && lastMoveEndCoords.y == 4)
                {

                    if (coords.y == 4 && lastMoveEndCoords.y == coords.y)
                    {
                        if (lastMoveEndCoords.x == coords.x + 1 || lastMoveEndCoords.x == coords.x - 1)
                        {
                            Vec2 enPassantTarget = {lastMoveEndCoords.x, coords.y + 1};
                            int enPassantIndex = vec2ToIndex(enPassantTarget);
                            if (board.state[enPassantIndex] == 0)
                            {
                                possibleMoves.push_back({board.state[index], index, enPassantIndex});
                            }
                        }
                    }
                }
            }
        }
        else if (is_lower(board.state[index]) && !board.isWhiteTurn)
        {

            Vec2 coords = indexToVec2(index);

            Vec2 downCoords = {coords.x, coords.y - 1};
            int downIndex = vec2ToIndex(downCoords);

            Vec2 downDownCoords = {coords.x, coords.y - 2};
            int downDownIndex = vec2ToIndex(downDownCoords);

            Vec2 downRight = {coords.x + 1, coords.y - 1};
            int downRightIndex = vec2ToIndex(downRight);

            Vec2 downLeft = {coords.x - 1, coords.y - 1};
            int downLeftIndex = vec2ToIndex(downLeft);

            if (downRight.x <= 7 && is_upper(board.state[downRightIndex]))
            {
                possibleMoves.push_back({board.state[index], index, downRightIndex});
            }

            if (downLeft.x >= 0 && is_upper(board.state[downLeftIndex]))
            {
                possibleMoves.push_back({board.state[index], index, downLeftIndex});
            }

            // TODO: change this logic to account for special moves
            if (board.state[downIndex] != 0)
                return possibleMoves;

            if (downCoords.y >= 1)
            {
                possibleMoves.push_back({board.state[index], index, downIndex});
            }

            if (coords.y == 6 && board.state[downDownIndex] == 0)
            {
                possibleMoves.push_back({board.state[index], index, downDownIndex});
            }

            if (!board.history.empty())
            {
                Move &lastMove = board.history.back();
                Vec2 lastMoveStartCoords = indexToVec2(lastMove.start);
                Vec2 lastMoveEndCoords = indexToVec2(lastMove.end);

                if (to_lower(lastMove.piece) == 'p' && lastMoveStartCoords.y == 1 && lastMoveEndCoords.y == 3)
                {
                    if (coords.y == 3 && lastMoveEndCoords.y == coords.y)
                    {
                        if (lastMoveEndCoords.x == coords.x + 1 || lastMoveEndCoords.x == coords.x - 1)
                        {
                            Vec2 enPassantTarget = {lastMoveEndCoords.x, coords.y - 1};
                            int enPassantIndex = vec2ToIndex(enPassantTarget);
                            if (board.state[enPassantIndex] == 0)
                            {
                                possibleMoves.push_back({board.state[index], index, enPassantIndex});
                            }
                        }
                    }
                }
            }
        }

        return possibleMoves;
    }

    std::vector<Move> MoveGen::GetKnightMoves(Board &board, int index)
    {

        std::vector<Move> possibleMoves;

        char piece = board.state[index];
        if (piece == 0)
            return possibleMoves;

        bool isWhitePiece = is_upper(piece);
        if (board.isWhiteTurn != isWhitePiece)
            return possibleMoves;

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
            char targetPiece = board.state[targetIndex];

            if (targetPiece == 0 ||
                (isWhitePiece && is_lower(targetPiece)) ||
                (!isWhitePiece && is_upper(targetPiece)))
            {
                possibleMoves.push_back({piece, index, targetIndex});
            }
        }

        return possibleMoves;
    }

    std::vector<Move> MoveGen::GetBishopMoves(Board &board, int index)
    {

        std::vector<Move> possibleMoves;

        char piece = board.state[index];
        if (piece == 0)
            return possibleMoves;

        bool isWhitePiece = is_upper(piece);
        if (board.isWhiteTurn != isWhitePiece)
            return possibleMoves;

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
                char targetPiece = board.state[targetIndex];

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

    std::vector<Move> MoveGen::GetRookMoves(Board &board, int index)
    {

        std::vector<Move> possibleMoves;

        char piece = board.state[index];
        if (piece == 0)
            return possibleMoves;

        bool isWhitePiece = is_upper(piece);
        if (board.isWhiteTurn != isWhitePiece)
            return possibleMoves;

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
                char targetPiece = board.state[targetIndex];

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

    std::vector<Move> MoveGen::GetQueenMoves(Board &board, int index)
    {

        std::vector<Move> possibleMoves;

        char piece = board.state[index];
        if (piece == 0)
            return possibleMoves;

        bool isWhitePiece = is_upper(piece);
        if (board.isWhiteTurn != isWhitePiece)
            return possibleMoves;

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
                char targetPiece = board.state[targetIndex];

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

    std::vector<Move> MoveGen::GetKingMoves(Board &board, int index)
    {

        std::vector<Move> possibleMoves;

        char piece = board.state[index];
        if (piece == 0)
            return possibleMoves;

        bool isWhitePiece = is_upper(piece);
        if (board.isWhiteTurn != isWhitePiece)
            return possibleMoves;

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
            char targetPiece = board.state[targetIndex];

            if (targetPiece == 0 ||
                (isWhitePiece && is_lower(targetPiece)) ||
                (!isWhitePiece && is_upper(targetPiece)))
            {
                possibleMoves.push_back({piece, index, targetIndex});
            }
        }

        if (isWhitePiece && coords.x == 4 && coords.y == 0)
        {
            if (board.castlingRights[0])
            {
                int f1 = vec2ToIndex({5, 0});
                int g1 = vec2ToIndex({6, 0});
                if (board.state[f1] == 0 && board.state[g1] == 0)
                {
                    possibleMoves.push_back({piece, index, g1});
                }
            }

            if (board.castlingRights[1])
            {
                int b1 = vec2ToIndex({1, 0});
                int c1 = vec2ToIndex({2, 0});
                int d1 = vec2ToIndex({3, 0});
                if (board.state[b1] == 0 && board.state[c1] == 0 && board.state[d1] == 0)
                {
                    possibleMoves.push_back({piece, index, c1});
                }
            }
        }

        if (!isWhitePiece && coords.x == 4 && coords.y == 7)
        {
            if (board.castlingRights[2])
            {
                int f8 = vec2ToIndex({5, 7});
                int g8 = vec2ToIndex({6, 7});
                if (board.state[f8] == 0 && board.state[g8] == 0)
                {
                    possibleMoves.push_back({piece, index, g8});
                }
            }

            if (board.castlingRights[3])
            {
                int b8 = vec2ToIndex({1, 7});
                int c8 = vec2ToIndex({2, 7});
                int d8 = vec2ToIndex({3, 7});
                if (board.state[b8] == 0 && board.state[c8] == 0 && board.state[d8] == 0)
                {
                    possibleMoves.push_back({piece, index, c8});
                }
            }
        }

        return possibleMoves;
    }
}
