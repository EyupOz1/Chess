#include "BoardState.hpp"
#include "Utils/Utils.hpp"
#include "Core/SETTING.hpp"
#include "raylib.h"

void BoardState::setup(std::string fenString)
{
    this->FEN(fenString);
}

void BoardState::movePiece(int startIndex, int targetIndex)
{
    uint8_t temp = this->board[startIndex]; 
    this->board[startIndex] = NONE;
    this->board[targetIndex] = temp; 
}





void BoardState::FEN(std::string fen)
{

    bool hasTurn = false;
    int boardIndex = 0;
    int fenIndex = 0;
    for (fenIndex = 0; fenIndex < fen.size() && (hasTurn = fen[fenIndex] != ' '); fenIndex++)
    {
        char curr = fen[fenIndex];

        uint8_t resultingPiece = 0;
        if ('A' <= curr && curr <= 'z')
        {
            resultingPiece = fenToPiece(curr);
            this->board[boardIndex++] = resultingPiece;
            continue;
        }

        if (curr == '/')
        {
            while (boardIndex % 8 != 0)
            {
                boardIndex++;
            }
            continue;
        }

        else if ('0' <= curr && curr <= '9')
        {
            boardIndex += (curr - 48);
        }
    }

    if (hasTurn)
    {
        TraceLog(2, "%c", fen[fenIndex + 1]);
    }
    
}



std::array<uint8_t, 64> BoardState::getBoard()
{
    return this->board;
}