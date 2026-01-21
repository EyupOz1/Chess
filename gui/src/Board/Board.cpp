#include "Board.hpp"
#include "chess/Utils.hpp"

GUI::Board::Board(float tile_size, Vector2 center, bool flipped)
{

    this->boardSize = 8;
    this->tileSize = tile_size;
    this->flipped = flipped;
    float half = (this->boardSize * this->tileSize) * 0.5f;
    this->origin = {center.x - half, center.y - half};
}


void GUI::Board::DrawBoard()
{
    const Color light = {235, 236, 208, 255};
    const Color dark = {119, 149, 86, 255};
    bool is_light = true;

    for (int rank = 0; rank < this->boardSize; ++rank)
    {
        for (int file = 0; file < this->boardSize; ++file)
        {

            Color current = is_light ? light : dark;
            is_light = !is_light;

            float x = this->origin.x + file * this->tileSize;
            float y = this->origin.y + rank * this->tileSize;

            DrawRectangle((int)x, (int)y, (int)this->tileSize, (int)this->tileSize, current);
        }
        is_light = !is_light;
    }
}

void GUI::Board::HighlightCells(const std::vector<Engine::Move> &moves)
{
    
    for (size_t i = 0; i < moves.size(); i++)
    {
        int targetIndex = moves[i].end;
        Engine::Vec2 coords = Engine::indexToVec2(targetIndex);
        if (this->flipped)
        {
            coords.x = (this->boardSize - 1) - coords.x;
            coords.y = (this->boardSize - 1) - coords.y;
        }
        int screenRank = (this->boardSize - 1) - coords.y;
        float x = this->origin.x + coords.x * this->tileSize;
        float y = this->origin.y + screenRank * this->tileSize;
        DrawRectangle(x, y, (int)this->tileSize, (int)this->tileSize, GREEN);
    }
}
