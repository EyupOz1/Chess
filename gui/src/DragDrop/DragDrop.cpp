#include "DragDrop.hpp"
#include "chess/Utils.hpp"

#include "raymath.h"

#include <cctype>
#include <cmath>



DragDropSystem::DragDropSystem()
{
    drag_ = {};
}

bool DragDropSystem::WorldToIndex(Vector2 world, const BoardView &view, int &out_index)
{
    float fx = (world.x - view.origin.x) / view.tile_size;
    float fy = (world.y - view.origin.y) / view.tile_size;
    int file = static_cast<int>(floorf(fx));
    int rank = static_cast<int>(floorf(fy));
    if (!IsOnBoard(file, rank, view.board_size))
    {
        return false;
    }
    if (view.flipped)
    {
        file = (view.board_size - 1) - file;
        rank = (view.board_size - 1) - rank;
    }
    out_index = IndexFromFileRank(file, rank, view.board_size);
    return true;
}

Vector2 DragDropSystem::IndexToWorldCenter(int index, const BoardView &view)
{
    int file = FileFromIndex(index, view.board_size);
    int rank = RankFromIndex(index, view.board_size);
    if (view.flipped)
    {
        file = (view.board_size - 1) - file;
        rank = (view.board_size - 1) - rank;
    }
    float x = view.origin.x + file * view.tile_size + view.tile_size * 0.5f;
    float y = view.origin.y + rank * view.tile_size + view.tile_size * 0.5f;
    return {x, y};
}



void DragDropSystem::Update(Board &board, const BoardView &view, const Camera2D &camera)
{
    Vector2 mouse_world = GetScreenToWorld2D(GetMousePosition(), camera);
    drag_.last_mouse_world = mouse_world;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        int index = -1;
        if (WorldToIndex(mouse_world, view, index))
        {
            char piece = board.state[index];
            if (IsFriendlyPiece(board.isWhiteTurn, piece))
            {
                drag_.active = true;
                drag_.source_index = index;
                drag_.target_index = index;
                drag_.piece = piece;
                Vector2 center = IndexToWorldCenter(index, view);
                // Keep the piece anchored relative to the cursor during drag.
                drag_.grab_offset = Vector2Subtract(center, mouse_world);
            }
        }
    }

    if (drag_.active)
    {
        int index = -1;
        if (WorldToIndex(mouse_world, view, index))
        {
            drag_.target_index = index;
        }
        else
        {
            drag_.target_index = -1;
        }
    }

    if (drag_.active && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        bool moved = false;
        if (drag_.target_index >= 0 && drag_.target_index != drag_.source_index)
        {
            char target_piece = board.state[drag_.target_index];
            if (target_piece == 0 || IsOpponentPiece(board.isWhiteTurn, target_piece))
            {
                board.forceMove(drag_.source_index, drag_.target_index);
                board.isWhiteTurn = !board.isWhiteTurn;
                moved = true;
            }
        }

        if (!moved)
        {
            board.state[drag_.source_index] = drag_.piece;
        }

        drag_.active = false;
        drag_.source_index = -1;
        drag_.target_index = -1;
        drag_.piece = 0;
    }
}

DragDropSystem::DragView DragDropSystem::GetDragView() const
{
    DragView view = {};
    view.active = drag_.active;
    view.source_index = drag_.source_index;
    view.target_index = drag_.target_index;
    view.piece = drag_.piece;
    view.world_pos = drag_.last_mouse_world;
    view.grab_offset = drag_.grab_offset;
    return view;
}

