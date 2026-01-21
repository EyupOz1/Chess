#include "chess/Board.hpp"
#include "chess/MoveGen.hpp"
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
        board.state.fill(0);
        board.history.clear();
        board.isWhiteTurn = true;

        int whitePawn = idx(4, 4); // e5
        int blackPawn = idx(3, 4); // d5
        board.state[whitePawn] = 'P';
        board.state[blackPawn] = 'p';
        board.history.push_back({'p', idx(3, 6), idx(3, 4)});

        Engine::MoveGen moveGen;
        std::vector<Engine::Move> moves = moveGen.GetPawnMoves(board, whitePawn);

        int enPassantTarget = idx(3, 5); // d6
        bool found = false;
        for (const Engine::Move &move : moves)
        {
            if (move.end == enPassantTarget)
                found = true;
        }
        expect(found, "en passant move exists");

        int result = board.Move(whitePawn, enPassantTarget);
        expect(result == 0, "en passant move allowed");
        expect(board.state[enPassantTarget] == 'P', "en passant moved pawn");
        expect(board.state[blackPawn] == 0, "en passant captured pawn");
    }

    {
        Engine::Board board;
        board.state.fill(0);
        board.history.clear();
        board.isWhiteTurn = true;
        board.castlingRights = {true, true, true, true};

        int whiteKing = idx(4, 0); // e1
        int whiteRook = idx(7, 0); // h1
        int castleTarget = idx(6, 0); // g1
        int rookTarget = idx(5, 0); // f1

        board.state[whiteKing] = 'K';
        board.state[whiteRook] = 'R';

        Engine::MoveGen moveGen;
        std::vector<Engine::Move> moves = moveGen.GetKingMoves(board, whiteKing);

        bool found = false;
        for (const Engine::Move &move : moves)
        {
            if (move.end == castleTarget)
                found = true;
        }
        expect(found, "castling move exists");

        int result = board.Move(whiteKing, castleTarget);
        expect(result == 0, "castling move allowed");
        expect(board.state[castleTarget] == 'K', "king castled");
        expect(board.state[rookTarget] == 'R', "rook moved on castle");
    }

    {
        Engine::Board board;
        board.state.fill(0);
        board.history.clear();
        board.isWhiteTurn = true;

        int whitePawn = idx(4, 4); // e5
        int blackPawn = idx(3, 4); // d5
        board.state[whitePawn] = 'P';
        board.state[blackPawn] = 'p';

        board.history.push_back({'p', idx(3, 6), idx(3, 4)});
        board.history.push_back({'N', idx(6, 0), idx(5, 2)});

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
        board.state.fill(0);
        board.history.clear();
        board.isWhiteTurn = true;
        board.castlingRights = {true, true, true, true};

        int whiteKing = idx(4, 0); // e1
        int whiteRookH = idx(7, 0); // h1
        int whiteRookA = idx(0, 0); // a1
        int blackRookA = idx(0, 7); // a8

        board.state[whiteKing] = 'K';
        board.state[whiteRookH] = 'R';
        board.state[whiteRookA] = 'R';
        board.state[blackRookA] = 'r';

        board.Move(whiteRookH, idx(6, 0));
        expect(board.castlingRights[0] == false, "king side right lost after rook move");

        board.isWhiteTurn = true;
        board.Move(whiteRookA, idx(0, 1));
        expect(board.castlingRights[1] == false, "queen side right lost after rook move");

        board.isWhiteTurn = true;
        board.Move(whiteKing, idx(4, 1));
        expect(board.castlingRights[0] == false && board.castlingRights[1] == false, "king move removes both rights");

        board.isWhiteTurn = false;
        board.Move(blackRookA, idx(0, 0));
        expect(board.castlingRights[1] == false, "rook capture removes queen side right");
    }

    {
        Engine::Board board;
        board.state.fill(0);
        board.history.clear();
        board.isWhiteTurn = true;

        int whitePawn = idx(4, 1); // e2
        int illegalTarget = idx(4, 3); // e4 (blocked)
        board.state[whitePawn] = 'P';

        board.state[idx(4, 2)] = 'p';
        int result = board.Move(whitePawn, illegalTarget);
        expect(result != 0, "illegal move rejected");
        expect(board.isWhiteTurn == true, "turn unchanged after illegal move");
    }

    if (failures == 0)
    {
        std::cout << "All tests passed\n";
        return 0;
    }

    std::cout << failures << " test(s) failed\n";
    return 1;
}
