#include "chess/MoveGen.hpp"
#include "chess/Utils.hpp"
#include <cmath>

namespace Engine
{

    MoveGenerator::MoveGenerator()
    {
        InitializeLookupTables();
    }

    void MoveGenerator::InitializeLookupTables()
    {
        InitializeKnightAttacks();
        InitializeKingAttacks();
    }

    void MoveGenerator::InitializeKnightAttacks()
    {
        knightAttacks_.fill(0);
        int knightMoves[8][2] = {
            {1, 2}, {2, 1}, {-1, 2}, {-2, 1},
            {1, -2}, {2, -1}, {-1, -2}, {-2, -1}
        };

        for (int square = 0; square < 64; ++square)
        {
            int file = fileFromIndex(square, 8);
            int rank = rankFromIndex(square, 8);

            for (const auto& move : knightMoves)
            {
                int newFile = file + move[0];
                int newRank = rank + move[1];
                if (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8)
                {
                    int targetSquare = indexFromFileRank(newFile, newRank, 8);
                    knightAttacks_[square] |= (1ULL << targetSquare);
                }
            }
        }
    }

    void MoveGenerator::InitializeKingAttacks()
    {
        kingAttacks_.fill(0);
        int kingMoves[8][2] = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1},
            {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
        };

        for (int square = 0; square < 64; ++square)
        {
            int file = fileFromIndex(square, 8);
            int rank = rankFromIndex(square, 8);

            for (const auto& move : kingMoves)
            {
                int newFile = file + move[0];
                int newRank = rank + move[1];
                if (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8)
                {
                    int targetSquare = indexFromFileRank(newFile, newRank, 8);
                    kingAttacks_[square] |= (1ULL << targetSquare);
                }
            }
        }
    }

    std::vector<Move> MoveGenerator::GetLegalMoves(const Position& pos)
    {
        std::vector<Move> moves = GetPseudoLegalMoves(pos);
        std::vector<Move> legalMoves;

        for (const auto& move : moves)
        {
            // TODO: Check if move leaves king in check
            legalMoves.push_back(move);
        }

        return legalMoves;
    }

    std::vector<Move> MoveGenerator::GetLegalMoves(const Position& pos, int fromSquare)
    {
        std::vector<Move> moves = GetPseudoLegalMoves(pos, fromSquare);
        std::vector<Move> legalMoves;

        for (const auto& move : moves)
        {
            // TODO: Check if move leaves king in check
            legalMoves.push_back(move);
        }

        return legalMoves;
    }

    std::vector<Move> MoveGenerator::GetPseudoLegalMoves(const Position& pos)
    {
        std::vector<Move> moves;

        for (int square = 0; square < 64; ++square)
        {
            auto squareMoves = GetPseudoLegalMoves(pos, square);
            moves.insert(moves.end(), squareMoves.begin(), squareMoves.end());
        }

        return moves;
    }

    std::vector<Move> MoveGenerator::GetPseudoLegalMoves(const Position& pos, int fromSquare)
    {
        std::vector<Move> moves;

        char piece = pos.PieceAt(fromSquare);
        if (piece == 0)
            return moves;

        // Check whose turn it is
        bool isWhite = is_upper(piece);
        if (isWhite != pos.IsWhiteToMove())
            return moves;

        char pieceLower = to_lower(piece);

        switch (pieceLower)
        {
            case 'p': return GetPawnMoves(pos, fromSquare);
            case 'n': return GetKnightMoves(pos, fromSquare);
            case 'b': return GetBishopMoves(pos, fromSquare);
            case 'r': return GetRookMoves(pos, fromSquare);
            case 'q': return GetQueenMoves(pos, fromSquare);
            case 'k': return GetKingMoves(pos, fromSquare);
            default: return moves;
        }
    }

    std::vector<Move> MoveGenerator::GetPawnMoves(const Position& pos, int fromSquare)
    {
        std::vector<Move> moves;
        char pawn = pos.PieceAt(fromSquare);
        if (pawn == 0 || to_lower(pawn) != 'p')
            return moves;

        bool isWhite = is_upper(pawn);
        int file = fileFromIndex(fromSquare, 8);
        int rank = rankFromIndex(fromSquare, 8);

        if (isWhite)
        {
            // Forward move
            int fwdSquare = indexFromFileRank(file, rank + 1, 8);
            if (rank + 1 < 8 && pos.PieceAt(fwdSquare) == 0)
            {
                AddPawnMove(moves, pos, fromSquare, fwdSquare);

                // Double push from starting position
                if (rank == 1)
                {
                    int fwdSquare2 = indexFromFileRank(file, rank + 2, 8);
                    if (pos.PieceAt(fwdSquare2) == 0)
                    {
                        AddMove(moves, pos, fromSquare, fwdSquare2);
                    }
                }
            }

            // Captures
            if (file + 1 < 8 && rank + 1 < 8)
            {
                int captureSquare = indexFromFileRank(file + 1, rank + 1, 8);
                char target = pos.PieceAt(captureSquare);
                if (target != 0 && is_lower(target))
                {
                    AddPawnMove(moves, pos, fromSquare, captureSquare);
                }
            }
            if (file - 1 >= 0 && rank + 1 < 8)
            {
                int captureSquare = indexFromFileRank(file - 1, rank + 1, 8);
                char target = pos.PieceAt(captureSquare);
                if (target != 0 && is_lower(target))
                {
                    AddPawnMove(moves, pos, fromSquare, captureSquare);
                }
            }

            // En passant
            int epSquare = pos.GetEnPassantSquare();
            if (epSquare >= 0)
            {
                int epFile = fileFromIndex(epSquare, 8);
                int epRank = rankFromIndex(epSquare, 8);
                if (epRank == rank + 1 && std::abs(epFile - file) == 1)
                {
                    AddMove(moves, pos, fromSquare, epSquare, MoveType::EnPassant);
                }
            }
        }
        else
        {
            // Forward move (down for black)
            int fwdSquare = indexFromFileRank(file, rank - 1, 8);
            if (rank - 1 >= 0 && pos.PieceAt(fwdSquare) == 0)
            {
                AddPawnMove(moves, pos, fromSquare, fwdSquare);

                // Double push from starting position
                if (rank == 6)
                {
                    int fwdSquare2 = indexFromFileRank(file, rank - 2, 8);
                    if (pos.PieceAt(fwdSquare2) == 0)
                    {
                        AddMove(moves, pos, fromSquare, fwdSquare2);
                    }
                }
            }

            // Captures
            if (file + 1 < 8 && rank - 1 >= 0)
            {
                int captureSquare = indexFromFileRank(file + 1, rank - 1, 8);
                char target = pos.PieceAt(captureSquare);
                if (target != 0 && is_upper(target))
                {
                    AddPawnMove(moves, pos, fromSquare, captureSquare);
                }
            }
            if (file - 1 >= 0 && rank - 1 >= 0)
            {
                int captureSquare = indexFromFileRank(file - 1, rank - 1, 8);
                char target = pos.PieceAt(captureSquare);
                if (target != 0 && is_upper(target))
                {
                    AddPawnMove(moves, pos, fromSquare, captureSquare);
                }
            }

            // En passant
            int epSquare = pos.GetEnPassantSquare();
            if (epSquare >= 0)
            {
                int epFile = fileFromIndex(epSquare, 8);
                int epRank = rankFromIndex(epSquare, 8);
                if (epRank == rank - 1 && std::abs(epFile - file) == 1)
                {
                    AddMove(moves, pos, fromSquare, epSquare, MoveType::EnPassant);
                }
            }
        }

        return moves;
    }

    std::vector<Move> MoveGenerator::GetKnightMoves(const Position& pos, int fromSquare)
    {
        std::vector<Move> moves;
        char knight = pos.PieceAt(fromSquare);
        if (knight == 0)
            return moves;

        bool isWhite = is_upper(knight);
        uint64_t attacks = knightAttacks_[fromSquare];

        for (int toSquare = 0; toSquare < 64; ++toSquare)
        {
            if ((attacks & (1ULL << toSquare)) == 0)
                continue;

            char target = pos.PieceAt(toSquare);
            if (target == 0 || (isWhite && is_lower(target)) || (!isWhite && is_upper(target)))
            {
                AddMove(moves, pos, fromSquare, toSquare);
            }
        }

        return moves;
    }

    std::vector<Move> MoveGenerator::GetBishopMoves(const Position& pos, int fromSquare)
    {
        std::vector<Move> moves;
        char bishop = pos.PieceAt(fromSquare);
        if (bishop == 0)
            return moves;

        bool isWhite = is_upper(bishop);

        // Diagonal directions
        int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

        for (const auto& dir : directions)
        {
            uint64_t rayAttacks = GetRayAttacks(fromSquare, pos, dir[0], dir[1]);
            for (int toSquare = 0; toSquare < 64; ++toSquare)
            {
                if ((rayAttacks & (1ULL << toSquare)) == 0)
                    continue;

                char target = pos.PieceAt(toSquare);
                if (target == 0 || (isWhite && is_lower(target)) || (!isWhite && is_upper(target)))
                {
                    AddMove(moves, pos, fromSquare, toSquare);
                }

                if (target != 0)
                    break;  // Stop at first piece
            }
        }

        return moves;
    }

    std::vector<Move> MoveGenerator::GetRookMoves(const Position& pos, int fromSquare)
    {
        std::vector<Move> moves;
        char rook = pos.PieceAt(fromSquare);
        if (rook == 0)
            return moves;

        bool isWhite = is_upper(rook);

        // Orthogonal directions
        int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

        for (const auto& dir : directions)
        {
            uint64_t rayAttacks = GetRayAttacks(fromSquare, pos, dir[0], dir[1]);
            for (int toSquare = 0; toSquare < 64; ++toSquare)
            {
                if ((rayAttacks & (1ULL << toSquare)) == 0)
                    continue;

                char target = pos.PieceAt(toSquare);
                if (target == 0 || (isWhite && is_lower(target)) || (!isWhite && is_upper(target)))
                {
                    AddMove(moves, pos, fromSquare, toSquare);
                }

                if (target != 0)
                    break;  // Stop at first piece
            }
        }

        return moves;
    }

    std::vector<Move> MoveGenerator::GetQueenMoves(const Position& pos, int fromSquare)
    {
        std::vector<Move> moves;
        char queen = pos.PieceAt(fromSquare);
        if (queen == 0)
            return moves;

        bool isWhite = is_upper(queen);

        // All 8 directions
        int directions[8][2] = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1},
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
        };

        for (const auto& dir : directions)
        {
            uint64_t rayAttacks = GetRayAttacks(fromSquare, pos, dir[0], dir[1]);
            for (int toSquare = 0; toSquare < 64; ++toSquare)
            {
                if ((rayAttacks & (1ULL << toSquare)) == 0)
                    continue;

                char target = pos.PieceAt(toSquare);
                if (target == 0 || (isWhite && is_lower(target)) || (!isWhite && is_upper(target)))
                {
                    AddMove(moves, pos, fromSquare, toSquare);
                }

                if (target != 0)
                    break;  // Stop at first piece
            }
        }

        return moves;
    }

    std::vector<Move> MoveGenerator::GetKingMoves(const Position& pos, int fromSquare)
    {
        std::vector<Move> moves;
        char king = pos.PieceAt(fromSquare);
        if (king == 0)
            return moves;

        bool isWhite = is_upper(king);
        uint64_t attacks = kingAttacks_[fromSquare];

        // Normal king moves
        for (int toSquare = 0; toSquare < 64; ++toSquare)
        {
            if ((attacks & (1ULL << toSquare)) == 0)
                continue;

            char target = pos.PieceAt(toSquare);
            if (target == 0 || (isWhite && is_lower(target)) || (!isWhite && is_upper(target)))
            {
                AddMove(moves, pos, fromSquare, toSquare);
            }
        }

        // Castling
        int rank = rankFromIndex(fromSquare, 8);
        if ((isWhite && rank == 0 && fromSquare == 4) ||
            (!isWhite && rank == 7 && fromSquare == 60))
        {
            const auto& castlingRights = pos.GetCastlingRights();

            // Kingside castling
            int castleIndex = isWhite ? 0 : 2;
            if (castlingRights[castleIndex])
            {
                int rookSquare = isWhite ? 7 : 63;
                int rook = pos.PieceAt(rookSquare);
                if ((isWhite && rook == 'R') || (!isWhite && rook == 'r'))
                {
                    int f = isWhite ? 5 : 61;
                    int g = isWhite ? 6 : 62;
                    if (pos.PieceAt(f) == 0 && pos.PieceAt(g) == 0)
                    {
                        AddMove(moves, pos, fromSquare, g, MoveType::Castle);
                    }
                }
            }

            // Queenside castling
            castleIndex = isWhite ? 1 : 3;
            if (castlingRights[castleIndex])
            {
                int rookSquare = isWhite ? 0 : 56;
                int rook = pos.PieceAt(rookSquare);
                if ((isWhite && rook == 'R') || (!isWhite && rook == 'r'))
                {
                    int b = isWhite ? 1 : 57;
                    int c = isWhite ? 2 : 58;
                    int d = isWhite ? 3 : 59;
                    if (pos.PieceAt(b) == 0 && pos.PieceAt(c) == 0 && pos.PieceAt(d) == 0)
                    {
                        AddMove(moves, pos, fromSquare, c, MoveType::Castle);
                    }
                }
            }
        }

        return moves;
    }

    bool MoveGenerator::IsSquareAttacked(const Position& pos, int square, bool byWhite) const
    {
        // TODO: Implement efficiently using lookup tables
        return false;
    }

    uint64_t MoveGenerator::GetAttackedSquares(const Position& pos, bool byWhite) const
    {
        // TODO: Implement
        return 0;
    }

    uint64_t MoveGenerator::GetPawnAttackSquares(int square, bool isWhite) const
    {
        // TODO: Implement
        return 0;
    }

    uint64_t MoveGenerator::GetRayAttacks(int square, const Position& pos, int dirX, int dirY) const
    {
        uint64_t attacks = 0;
        int file = fileFromIndex(square, 8);
        int rank = rankFromIndex(square, 8);

        file += dirX;
        rank += dirY;

        while (file >= 0 && file < 8 && rank >= 0 && rank < 8)
        {
            int targetSquare = indexFromFileRank(file, rank, 8);
            attacks |= (1ULL << targetSquare);

            if (pos.PieceAt(targetSquare) != 0)
                break;

            file += dirX;
            rank += dirY;
        }

        return attacks;
    }

    void MoveGenerator::AddMove(std::vector<Move>& moves, const Position& pos, int from, int to,
                               MoveType type, char promotion) const
    {
        Move move;
        move.from = from;
        move.to = to;
        move.type = type;
        move.pieceMoved = pos.PieceAt(from);
        move.pieceCaptured = pos.PieceAt(to);
        move.promotion = promotion;
        moves.push_back(move);
    }

    void MoveGenerator::AddPawnMove(std::vector<Move>& moves, const Position& pos, int from, int to) const
    {
        int rank = rankFromIndex(to, 8);
        bool isPromotionRank = (rank == 7) || (rank == 0);

        if (!isPromotionRank)
        {
            AddMove(moves, pos, from, to);
            return;
        }

        // Promotion moves
        char promoPieces[4] = {'Q', 'R', 'B', 'N'};
        for (char promo : promoPieces)
        {
            char promoChar = is_upper(pos.PieceAt(from)) ? promo : to_lower(promo);
            Move move;
            move.from = from;
            move.to = to;
            move.type = MoveType::Promotion;
            move.pieceMoved = pos.PieceAt(from);
            move.pieceCaptured = pos.PieceAt(to);
            move.promotion = promoChar;
            moves.push_back(move);
        }
    }

    // Legacy compatibility: MoveGen adapter for Board-based API
    std::vector<Move> MoveGen::GetPseudoLegalMoves(const Board &board, int index)
    {
        // Convert Board to Position
        Position pos;
        for (int i = 0; i < 64; ++i)
            pos.pieces[i] = board.PieceAt(i);
        pos.whiteToMove = board.IsWhiteTurn();
        pos.castlingRights = board.CastlingRights();
        pos.enPassantSquare = board.EnPassantIndex();
        pos.halfMoveClock = board.HalfMoves();
        pos.fullMoveNumber = board.FullMoves();

        return generator.GetPseudoLegalMoves(pos, index);
    }

    std::vector<Move> MoveGen::GetPawnMoves(const Board &board, int index)
    {
        Position pos;
        for (int i = 0; i < 64; ++i)
            pos.pieces[i] = board.PieceAt(i);
        pos.whiteToMove = board.IsWhiteTurn();
        pos.castlingRights = board.CastlingRights();
        pos.enPassantSquare = board.EnPassantIndex();
        return generator.GetPawnMoves(pos, index);
    }

    std::vector<Move> MoveGen::GetKnightMoves(const Board &board, int index)
    {
        Position pos;
        for (int i = 0; i < 64; ++i)
            pos.pieces[i] = board.PieceAt(i);
        pos.whiteToMove = board.IsWhiteTurn();
        return generator.GetKnightMoves(pos, index);
    }

    std::vector<Move> MoveGen::GetBishopMoves(const Board &board, int index)
    {
        Position pos;
        for (int i = 0; i < 64; ++i)
            pos.pieces[i] = board.PieceAt(i);
        pos.whiteToMove = board.IsWhiteTurn();
        return generator.GetBishopMoves(pos, index);
    }

    std::vector<Move> MoveGen::GetRookMoves(const Board &board, int index)
    {
        Position pos;
        for (int i = 0; i < 64; ++i)
            pos.pieces[i] = board.PieceAt(i);
        pos.whiteToMove = board.IsWhiteTurn();
        return generator.GetRookMoves(pos, index);
    }

    std::vector<Move> MoveGen::GetQueenMoves(const Board &board, int index)
    {
        Position pos;
        for (int i = 0; i < 64; ++i)
            pos.pieces[i] = board.PieceAt(i);
        pos.whiteToMove = board.IsWhiteTurn();
        return generator.GetQueenMoves(pos, index);
    }

    std::vector<Move> MoveGen::GetKingMoves(const Board &board, int index)
    {
        Position pos;
        for (int i = 0; i < 64; ++i)
            pos.pieces[i] = board.PieceAt(i);
        pos.whiteToMove = board.IsWhiteTurn();
        pos.castlingRights = board.CastlingRights();
        return generator.GetKingMoves(pos, index);
    }

}
