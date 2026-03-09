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
        std::vector<Move> pseudoMoves = GetPseudoLegalMoves(pos);
        std::vector<Move> legalMoves;
        bool isWhite = pos.IsWhiteToMove();
        char kingChar = isWhite ? 'K' : 'k';

        for (const auto& move : pseudoMoves)
        {
            // Apply move to a temporary copy and check king safety
            Position copy = pos;
            copy.SetPiece(move.from, 0);
            copy.SetPiece(move.to, move.promotion ? move.promotion : move.pieceMoved);

            if (move.type == MoveType::EnPassant)
            {
                // Captured pawn is on the same rank as the moving pawn, same file as destination
                int capturedPawn = indexFromFileRank(
                    fileFromIndex(move.to, 8), rankFromIndex(move.from, 8), 8);
                copy.SetPiece(capturedPawn, 0);
            }
            else if (move.type == MoveType::Castle)
            {
                int r = rankFromIndex(move.from, 8);
                if (fileFromIndex(move.to, 8) == 6) // Kingside
                {
                    copy.SetPiece(indexFromFileRank(7, r, 8), 0);
                    copy.SetPiece(indexFromFileRank(5, r, 8), isWhite ? 'R' : 'r');
                }
                else // Queenside
                {
                    copy.SetPiece(indexFromFileRank(0, r, 8), 0);
                    copy.SetPiece(indexFromFileRank(3, r, 8), isWhite ? 'R' : 'r');
                }
            }

            int kingSquare = -1;
            for (int sq = 0; sq < 64; ++sq)
                if (copy.PieceAt(sq) == kingChar) { kingSquare = sq; break; }

            if (kingSquare >= 0 && !IsSquareAttacked(copy, kingSquare, !isWhite))
                legalMoves.push_back(move);
        }

        return legalMoves;
    }

    std::vector<Move> MoveGenerator::GetLegalMoves(const Position& pos, int fromSquare)
    {
        std::vector<Move> all = GetLegalMoves(pos);
        std::vector<Move> filtered;
        for (const auto& move : all)
            if (move.from == fromSquare)
                filtered.push_back(move);
        return filtered;
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
        int file = fileFromIndex(fromSquare, 8);
        int rank = rankFromIndex(fromSquare, 8);

        int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (const auto& dir : directions)
        {
            int f = file + dir[0], r = rank + dir[1];
            while (f >= 0 && f < 8 && r >= 0 && r < 8)
            {
                int toSquare = indexFromFileRank(f, r, 8);
                char target = pos.PieceAt(toSquare);
                if (target == 0)
                {
                    AddMove(moves, pos, fromSquare, toSquare);
                }
                else
                {
                    if ((isWhite && is_lower(target)) || (!isWhite && is_upper(target)))
                        AddMove(moves, pos, fromSquare, toSquare);
                    break;
                }
                f += dir[0];
                r += dir[1];
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
        int file = fileFromIndex(fromSquare, 8);
        int rank = rankFromIndex(fromSquare, 8);

        int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        for (const auto& dir : directions)
        {
            int f = file + dir[0], r = rank + dir[1];
            while (f >= 0 && f < 8 && r >= 0 && r < 8)
            {
                int toSquare = indexFromFileRank(f, r, 8);
                char target = pos.PieceAt(toSquare);
                if (target == 0)
                {
                    AddMove(moves, pos, fromSquare, toSquare);
                }
                else
                {
                    if ((isWhite && is_lower(target)) || (!isWhite && is_upper(target)))
                        AddMove(moves, pos, fromSquare, toSquare);
                    break;
                }
                f += dir[0];
                r += dir[1];
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
        int file = fileFromIndex(fromSquare, 8);
        int rank = rankFromIndex(fromSquare, 8);

        int directions[8][2] = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1},
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
        };
        for (const auto& dir : directions)
        {
            int f = file + dir[0], r = rank + dir[1];
            while (f >= 0 && f < 8 && r >= 0 && r < 8)
            {
                int toSquare = indexFromFileRank(f, r, 8);
                char target = pos.PieceAt(toSquare);
                if (target == 0)
                {
                    AddMove(moves, pos, fromSquare, toSquare);
                }
                else
                {
                    if ((isWhite && is_lower(target)) || (!isWhite && is_upper(target)))
                        AddMove(moves, pos, fromSquare, toSquare);
                    break;
                }
                f += dir[0];
                r += dir[1];
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
        int file = fileFromIndex(square, 8);
        int rank = rankFromIndex(square, 8);

        // Pawns
        char pawnChar = byWhite ? 'P' : 'p';
        int pawnDir = byWhite ? -1 : 1;
        for (int df : {-1, 1})
        {
            int f = file + df, r = rank + pawnDir;
            if (f >= 0 && f < 8 && r >= 0 && r < 8)
                if (pos.PieceAt(indexFromFileRank(f, r, 8)) == pawnChar)
                    return true;
        }

        // Knights (use pre-computed table)
        char knightChar = byWhite ? 'N' : 'n';
        uint64_t knightBits = knightAttacks_[square];
        for (int sq = 0; sq < 64; ++sq)
            if ((knightBits >> sq) & 1)
                if (pos.PieceAt(sq) == knightChar)
                    return true;

        // King (use pre-computed table)
        char kingChar = byWhite ? 'K' : 'k';
        uint64_t kingBits = kingAttacks_[square];
        for (int sq = 0; sq < 64; ++sq)
            if ((kingBits >> sq) & 1)
                if (pos.PieceAt(sq) == kingChar)
                    return true;

        // Orthogonal rays (rook or queen)
        char rookChar  = byWhite ? 'R' : 'r';
        char queenChar = byWhite ? 'Q' : 'q';
        int orthoDirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        for (const auto& dir : orthoDirs)
        {
            int f = file + dir[0], r = rank + dir[1];
            while (f >= 0 && f < 8 && r >= 0 && r < 8)
            {
                char piece = pos.PieceAt(indexFromFileRank(f, r, 8));
                if (piece != 0)
                {
                    if (piece == rookChar || piece == queenChar) return true;
                    break;
                }
                f += dir[0]; r += dir[1];
            }
        }

        // Diagonal rays (bishop or queen)
        char bishopChar = byWhite ? 'B' : 'b';
        int diagDirs[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (const auto& dir : diagDirs)
        {
            int f = file + dir[0], r = rank + dir[1];
            while (f >= 0 && f < 8 && r >= 0 && r < 8)
            {
                char piece = pos.PieceAt(indexFromFileRank(f, r, 8));
                if (piece != 0)
                {
                    if (piece == bishopChar || piece == queenChar) return true;
                    break;
                }
                f += dir[0]; r += dir[1];
            }
        }

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
