#include "PieceRenderer.hpp"

#include "raymath.h"

#include <cctype>

static Vector2 IndexToWorldTopLeft(int index, const BoardView &view)
{
    int file = FileFromIndex(index, view.board_size);
    int rank = RankFromIndex(index, view.board_size);
    if (view.flipped)
    {
        file = (view.board_size - 1) - file;
        rank = (view.board_size - 1) - rank;
    }
    float x = view.origin.x + file * view.tile_size;
    float y = view.origin.y + rank * view.tile_size;
    return {x, y};
}

static Vector2 IndexToWorldCenter(int index, const BoardView &view)
{
    Vector2 top_left = IndexToWorldTopLeft(index, view);
    return {top_left.x + view.tile_size * 0.5f, top_left.y + view.tile_size * 0.5f};
}

static Color GetPieceFill(char piece)
{
    if (std::isupper(static_cast<unsigned char>(piece)) != 0)
    {
        return {245, 245, 245, 255};
    }
    return {40, 40, 40, 255};
}

static Color GetPieceTextColor(char piece)
{
    if (std::isupper(static_cast<unsigned char>(piece)) != 0)
    {
        return {20, 20, 20, 255};
    }
    return {235, 235, 235, 255};
}

static void DrawPieceGlyph(char piece, Vector2 center, float tile_size)
{
    char glyph[2] = {static_cast<char>(std::toupper(static_cast<unsigned char>(piece))), '\0'};
    int font_size = static_cast<int>(tile_size * 0.5f);
    int text_width = MeasureText(glyph, font_size);
    DrawText(glyph,
             static_cast<int>(center.x - text_width * 0.5f),
             static_cast<int>(center.y - font_size * 0.5f),
             font_size,
             GetPieceTextColor(piece));
}

static void DrawPiece(char piece, Vector2 center, float tile_size)
{
    DrawCircleV(center, tile_size * 0.35f, GetPieceFill(piece));
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), tile_size * 0.35f, {0, 0, 0, 80});
    DrawPieceGlyph(piece, center, tile_size);
}

void DrawPieces(const Board &board, const BoardView &view, const DragDropSystem::DragView &drag)
{
    if (drag.active && drag.target_index >= 0)
    {
        Vector2 top_left = IndexToWorldTopLeft(drag.target_index, view);
        // Show the snapped target square while dragging.
        DrawRectangleLines(static_cast<int>(top_left.x),
                           static_cast<int>(top_left.y),
                           static_cast<int>(view.tile_size),
                           static_cast<int>(view.tile_size),
                           YELLOW);
    }

    int total_squares = view.board_size * view.board_size;
    for (int index = 0; index < total_squares; ++index)
    {
        char piece = board.state[index];
        if (piece == 0)
        {
            continue;
        }
        if (drag.active && index == drag.source_index)
        {
            continue;
        }
        Vector2 center = IndexToWorldCenter(index, view);
        DrawPiece(piece, center, view.tile_size);
    }

    if (drag.active && drag.piece != 0)
    {
        Vector2 center = Vector2Add(drag.world_pos, drag.grab_offset);
        DrawPiece(drag.piece, center, view.tile_size);
    }
}
