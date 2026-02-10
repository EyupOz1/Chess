#include "Board.hpp"
#include "chess/Utils.hpp"

GUI::Board::Board(float tile_size, Vector2 center, bool flipped)
{

    this->boardSize = 8;
    this->tileSize = tile_size;
    this->flipped = flipped;
    float half = (this->boardSize * this->tileSize) * 0.5f;
    this->origin = {center.x - half, center.y - half};
    this->lightColor = {240, 238, 230, 255};
    this->darkColor = {94, 120, 86, 255};
    this->borderColor = {30, 30, 30, 180};
}


void GUI::Board::DrawBoard()
{
    bool is_light = true;

    for (int rank = 0; rank < this->boardSize; ++rank)
    {
        for (int file = 0; file < this->boardSize; ++file)
        {

            Color current = is_light ? this->lightColor : this->darkColor;
            is_light = !is_light;

            float x = this->origin.x + file * this->tileSize;
            float y = this->origin.y + rank * this->tileSize;

            DrawRectangle((int)x, (int)y, (int)this->tileSize, (int)this->tileSize, current);
        }
        is_light = !is_light;
    }

    DrawRectangleLines((int)this->origin.x,
                       (int)this->origin.y,
                       (int)(this->boardSize * this->tileSize),
                       (int)(this->boardSize * this->tileSize),
                       this->borderColor);
}

void GUI::Board::HighlightCells(const std::vector<Engine::Move> &moves)
{
    const Color highlight = {60, 140, 95, 130};
    for (size_t i = 0; i < moves.size(); i++)
    {
        int targetIndex = moves[i].to;
        Engine::Vec2 coords = Engine::indexToVec2(targetIndex);
        if (this->flipped)
        {
            coords.x = (this->boardSize - 1) - coords.x;
            coords.y = (this->boardSize - 1) - coords.y;
        }
        int screenRank = (this->boardSize - 1) - coords.y;
        float x = this->origin.x + coords.x * this->tileSize;
        float y = this->origin.y + screenRank * this->tileSize;
        DrawRectangle(x, y, (int)this->tileSize, (int)this->tileSize, highlight);
    }
}
