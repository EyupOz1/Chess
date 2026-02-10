#include "chess/Position.hpp"
#include "chess/Utils.hpp"
#include <sstream>
#include <cstring>

namespace Engine
{

    char Position::PieceAt(int square) const
    {
        if (!IsValidSquare(square))
            return 0;
        return pieces[square];
    }

    void Position::SetPiece(int square, char piece)
    {
        if (!IsValidSquare(square))
            return;
        pieces[square] = piece;
    }

    void Position::Clear()
    {
        pieces.fill(0);
        whiteToMove = true;
        castlingRights = {};
        enPassantSquare = -1;
        halfMoveClock = 0;
        fullMoveNumber = 1;
        zobristHash = 0;
    }

    bool Position::LoadFromFEN(const std::string& fen)
    {
        Clear();

        std::istringstream iss(fen);
        std::string boardPart, turnPart, castlingPart, enPassantPart;
        int halfMoves = 0, fullMoves = 1;

        if (!(iss >> boardPart >> turnPart >> castlingPart >> enPassantPart >> halfMoves >> fullMoves))
            return false;

        // Parse board
        int rank = kBoardSize - 1;
        int file = 0;
        for (char c : boardPart)
        {
            if (c == '/')
            {
                if (file != kBoardSize)
                    return false;
                rank--;
                file = 0;
                continue;
            }

            if (c >= '1' && c <= '8')
            {
                file += c - '0';
                if (file > kBoardSize)
                    return false;
                continue;
            }

            // Valid piece characters
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            {
                if (rank < 0 || file >= kBoardSize)
                    return false;
                int square = indexFromFileRank(file, rank, kBoardSize);
                pieces[square] = c;
                file++;
                continue;
            }

            return false;
        }

        if (rank != 0 || file != kBoardSize)
            return false;

        // Parse turn
        if (turnPart == "w")
            whiteToMove = true;
        else if (turnPart == "b")
            whiteToMove = false;
        else
            return false;

        // Parse castling rights
        castlingRights = {};
        if (castlingPart != "-")
        {
            castlingRights[0] = castlingPart.find('K') != std::string::npos;
            castlingRights[1] = castlingPart.find('Q') != std::string::npos;
            castlingRights[2] = castlingPart.find('k') != std::string::npos;
            castlingRights[3] = castlingPart.find('q') != std::string::npos;
        }

        // Parse en passant
        if (enPassantPart != "-")
        {
            if (enPassantPart.length() == 2)
            {
                int f = enPassantPart[0] - 'a';
                int r = enPassantPart[1] - '1';
                if (f >= 0 && f < kBoardSize && r >= 0 && r < kBoardSize)
                    enPassantSquare = indexFromFileRank(f, r, kBoardSize);
                else
                    return false;
            }
            else
                return false;
        }

        halfMoveClock = halfMoves;
        fullMoveNumber = fullMoves;

        return true;
    }

    std::string Position::ToFEN() const
    {
        std::string fen;

        // Board
        for (int rank = kBoardSize - 1; rank >= 0; rank--)
        {
            int emptyCount = 0;
            for (int file = 0; file < kBoardSize; file++)
            {
                char piece = PieceAt(indexFromFileRank(file, rank, kBoardSize));
                if (piece == 0)
                {
                    emptyCount++;
                }
                else
                {
                    if (emptyCount > 0)
                    {
                        fen += (char)('0' + emptyCount);
                        emptyCount = 0;
                    }
                    fen += piece;
                }
            }
            if (emptyCount > 0)
                fen += (char)('0' + emptyCount);
            if (rank > 0)
                fen += '/';
        }

        // Turn
        fen += (whiteToMove ? " w " : " b ");

        // Castling rights
        if (!castlingRights[0] && !castlingRights[1] && !castlingRights[2] && !castlingRights[3])
            fen += '-';
        else
        {
            if (castlingRights[0])
                fen += 'K';
            if (castlingRights[1])
                fen += 'Q';
            if (castlingRights[2])
                fen += 'k';
            if (castlingRights[3])
                fen += 'q';
        }

        // En passant
        fen += ' ';
        if (enPassantSquare >= 0)
        {
            int file = fileFromIndex(enPassantSquare, kBoardSize);
            int rank = rankFromIndex(enPassantSquare, kBoardSize);
            fen += (char)('a' + file);
            fen += (char)('1' + rank);
        }
        else
            fen += '-';

        // Move counters
        fen += ' ' + std::to_string(halfMoveClock) + ' ' + std::to_string(fullMoveNumber);

        return fen;
    }

    bool Position::operator==(const Position& other) const
    {
        return pieces == other.pieces &&
               whiteToMove == other.whiteToMove &&
               castlingRights == other.castlingRights &&
               enPassantSquare == other.enPassantSquare &&
               halfMoveClock == other.halfMoveClock &&
               fullMoveNumber == other.fullMoveNumber;
    }

}
