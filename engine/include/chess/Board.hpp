#pragma once

#include <string>
#include <array>
#include <vector>

#include "Utils.hpp"


struct Board {
    std::array<char, 64> state{};
    bool isWhiteTurn = true;
    std::array<bool, 4> castlingRights{};
    std::string enPassantTarget = "";

    std::vector<Move> history;

    int halfMoves = 0;
    int fullMoves = 1;

    int move();
    int forceMove(int sourceIndex, int targetIndex);

    int loadFen(std::string FEN);
    std::string exportFen();

    

    void printBoard();
    void printInfo();

    void possibleMoves();

};


