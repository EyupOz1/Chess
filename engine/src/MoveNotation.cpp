#include "chess/MoveNotation.hpp"
#include "chess/Utils.hpp"

namespace Engine
{

    std::string MoveNotation::ToUCI(const Move& move)
    {
        std::string uci;
        uci += SquareToAlgebraic(move.from);
        uci += SquareToAlgebraic(move.to);
        if (move.promotion != 0)
            uci += to_lower(move.promotion);
        return uci;
    }

    std::string MoveNotation::ToSAN(const Move& move, const Position& pos)
    {
        std::string san;

        // Handle castling
        if (move.type == MoveType::Castle)
        {
            int toFile = fileFromIndex(move.to, 8);
            return toFile == 6 ? "O-O" : "O-O-O";
        }

        // Handle pawn moves
        if (move.pieceMoved != 0 && to_lower(move.pieceMoved) == 'p')
        {
            // Pawn captures show source file
            if (move.IsCapture())
            {
                int fromFile = fileFromIndex(move.from, 8);
                san += (char)('a' + fromFile);
            }
            san += 'x';
            san += SquareToAlgebraic(move.to);
        }
        else
        {
            // Piece moves
            if (move.pieceMoved != 0)
                san += GetPieceSymbol(move.pieceMoved);

            // Capture notation
            if (move.IsCapture())
                san += 'x';

            san += SquareToAlgebraic(move.to);
        }

        // Promotion
        if (move.promotion != 0)
        {
            san += '=';
            san += GetPieceSymbol(move.promotion);
        }

        // Check/Checkmate would require full position evaluation after move
        // This would need to temporarily apply the move and evaluate
        // For now, we omit the + and # notation as it's not essential for move display
        // The GameController can handle check display separately

        return san;
    }

    std::string MoveNotation::ToLAN(const Move& move)
    {
        std::string lan;
        lan += SquareToAlgebraic(move.from);
        lan += '-';
        lan += SquareToAlgebraic(move.to);
        if (move.promotion != 0)
        {
            lan += '=';
            lan += to_upper(move.promotion);
        }
        return lan;
    }

    std::string MoveNotation::ToDescriptive(const Move& move)
    {
        std::string desc;
        if (move.pieceMoved != 0)
            desc += GetPieceSymbol(move.pieceMoved);
        desc += " from ";
        desc += SquareToAlgebraic(move.from);
        desc += " to ";
        desc += SquareToAlgebraic(move.to);
        if (move.IsCapture())
            desc += " (captures " + std::string(1, move.pieceCaptured) + ")";
        if (move.promotion != 0)
            desc += " (promotes to " + std::string(1, move.promotion) + ")";
        return desc;
    }

    Move MoveNotation::ParseUCI(const std::string& uci)
    {
        Move move;
        if (uci.length() < 4)
            return move;

        move.from = AlgebraicToSquare(uci.substr(0, 2));
        move.to = AlgebraicToSquare(uci.substr(2, 2));

        if (uci.length() >= 5)
            move.promotion = to_upper(uci[4]);

        return move;
    }

    char MoveNotation::GetPieceSymbol(char piece)
    {
        char lower = to_lower(piece);
        switch (lower)
        {
            case 'p': return ' ';  // Pawns have no symbol
            case 'n': return 'N';
            case 'b': return 'B';
            case 'r': return 'R';
            case 'q': return 'Q';
            case 'k': return 'K';
            default: return '?';
        }
    }

    std::string MoveNotation::SquareToAlgebraic(int square)
    {
        if (square < 0 || square >= 64)
            return "??";

        int file = fileFromIndex(square, 8);
        int rank = rankFromIndex(square, 8);
        std::string alg;
        alg += (char)('a' + file);
        alg += (char)('1' + rank);
        return alg;
    }

    int MoveNotation::AlgebraicToSquare(const std::string& alg)
    {
        if (alg.length() != 2)
            return -1;

        int file = alg[0] - 'a';
        int rank = alg[1] - '1';

        if (file < 0 || file >= 8 || rank < 0 || rank >= 8)
            return -1;

        return indexFromFileRank(file, rank, 8);
    }

}
