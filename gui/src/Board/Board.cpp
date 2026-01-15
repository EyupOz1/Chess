#include "Board.hpp"

BoardView CreateBoardView(float tile_size, Vector2 center, bool flipped)
{
    BoardView view = {};
    view.board_size = 8;
    view.tile_size = tile_size;
    view.flipped = flipped;
    float half = (view.board_size * view.tile_size) * 0.5f;
    view.origin = {center.x - half, center.y - half};
    return view;
}

void DrawChessBoard(const BoardView &view)
{
    const Color light = {235, 236, 208, 255};
    const Color dark = {119, 149, 86, 255};
    bool is_light = true;

    for (int rank = 0; rank < view.board_size; ++rank)
    {
        for (int file = 0; file < view.board_size; ++file)
        {
            Color current = is_light ? light : dark;
            is_light = !is_light;
            float x = view.origin.x + file * view.tile_size;
            float y = view.origin.y + rank * view.tile_size;
            DrawRectangle((int)x, (int)y, (int)view.tile_size, (int)view.tile_size, current);
        }
        is_light = !is_light;
    }
}
