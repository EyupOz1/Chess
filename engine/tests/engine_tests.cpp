#include "chess/Board.hpp"
#include "chess/MoveGen.hpp"
#include "chess/Rules.hpp"
#include "chess/Utils.hpp"

#include <iostream>
#include <vector>

namespace
{
    int failures = 0;

    void expect(bool condition, const char *message)
    {
        if (!condition)
        {
            std::cout << "FAIL: " << message << "\n";
            failures++;
        }
    }

    int idx(int file, int rank)
    {
        return Engine::indexFromFileRank(file, rank, 8);
    }
}

int main()
{
    {
        Engine::Board board;
        board.Clear();
        board.SetTurn(true);

        int whitePawn = idx(4, 4); // e5
        int blackPawn = idx(3, 4); // d5
        board.SetPiece(whitePawn, 'P');
        board.SetPiece(blackPawn, 'p');
        int enPassantTarget = idx(3, 5); // d6
        board.SetEnPassantIndex(enPassantTarget);

        Engine::MoveGen moveGen;
        std::vector<Engine::Move> moves = moveGen.GetPawnMoves(board, whitePawn);

        bool found = false;
        for (const Engine::Move &move : moves)
        {
            if (move.end == enPassantTarget)
                found = true;
        }
        expect(found, "en passant move exists");

        Engine::MoveResult result = board.TryMove(whitePawn, enPassantTarget);
        expect(result.Ok(), "en passant move allowed");
        expect(board.PieceAt(enPassantTarget) == 'P', "en passant moved pawn");
        expect(board.PieceAt(blackPawn) == 0, "en passant captured pawn");
    }

    {
        Engine::Board board;
        board.Clear();
        board.SetTurn(true);
        board.SetCastlingRights({true, true, true, true});

        int whiteKing = idx(4, 0); // e1
        int whiteRook = idx(7, 0); // h1
        int castleTarget = idx(6, 0); // g1
        int rookTarget = idx(5, 0); // f1

        board.SetPiece(whiteKing, 'K');
        board.SetPiece(whiteRook, 'R');

        Engine::MoveGen moveGen;
        std::vector<Engine::Move> moves = moveGen.GetKingMoves(board, whiteKing);

        bool found = false;
        for (const Engine::Move &move : moves)
        {
            if (move.end == castleTarget)
                found = true;
        }
        expect(found, "castling move exists");

        Engine::MoveResult result = board.TryMove(whiteKing, castleTarget);
        expect(result.Ok(), "castling move allowed");
        expect(board.PieceAt(castleTarget) == 'K', "king castled");
        expect(board.PieceAt(rookTarget) == 'R', "rook moved on castle");
    }

    {
        Engine::Board board;
        board.Clear();
        board.SetTurn(true);

        int whitePawn = idx(4, 4); // e5
        int blackPawn = idx(3, 4); // d5
        board.SetPiece(whitePawn, 'P');
        board.SetPiece(blackPawn, 'p');

        board.SetEnPassantIndex(-1);

        Engine::MoveGen moveGen;
        std::vector<Engine::Move> moves = moveGen.GetPawnMoves(board, whitePawn);

        int enPassantTarget = idx(3, 5); // d6
        bool found = false;
        for (const Engine::Move &move : moves)
        {
            if (move.end == enPassantTarget)
                found = true;
        }
        expect(!found, "en passant only allowed immediately");
    }

    {
        Engine::Board board;
        board.Clear();
        board.SetTurn(true);
        board.SetCastlingRights({true, true, true, true});

        int whiteKing = idx(4, 0); // e1
        int whiteRookH = idx(7, 0); // h1
        int whiteRookA = idx(0, 0); // a1
        int blackRookA = idx(0, 7); // a8

        board.SetPiece(whiteKing, 'K');
        board.SetPiece(whiteRookH, 'R');
        board.SetPiece(whiteRookA, 'R');
        board.SetPiece(blackRookA, 'r');

        board.TryMove(whiteRookH, idx(6, 0));
        expect(board.CastlingRights()[0] == false, "king side right lost after rook move");

        board.SetTurn(true);
        board.TryMove(whiteRookA, idx(0, 1));
        expect(board.CastlingRights()[1] == false, "queen side right lost after rook move");

        board.SetTurn(true);
        board.TryMove(whiteKing, idx(4, 1));
        expect(board.CastlingRights()[0] == false && board.CastlingRights()[1] == false, "king move removes both rights");

        board.SetTurn(false);
        board.TryMove(blackRookA, idx(0, 0));
        expect(board.CastlingRights()[1] == false, "rook capture removes queen side right");
    }

    {
        Engine::Board board;
        board.Clear();
        board.SetTurn(true);

        int whitePawn = idx(4, 1); // e2
        int illegalTarget = idx(4, 3); // e4 (blocked)
        board.SetPiece(whitePawn, 'P');

        board.SetPiece(idx(4, 2), 'p');
        Engine::MoveResult result = board.TryMove(whitePawn, illegalTarget);
        expect(!result.Ok(), "illegal move rejected");
        expect(board.IsWhiteTurn() == true, "turn unchanged after illegal move");
    }

    {
        Engine::Board board;
        int status = board.LoadFen("7k/6Q1/7K/8/8/8/8/8 b - - 0 1");
        expect(status == 0, "load checkmate FEN");
        expect(Engine::Rules::GetStatus(board) == Engine::PositionStatus::Checkmate, "checkmate detected");
    }

    if (failures == 0)
    {
        std::cout << "All tests passed\n";
        return 0;
    }

    std::cout << failures << " test(s) failed\n";
    return 1;
}
