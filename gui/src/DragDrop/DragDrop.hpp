#pragma once

#include "raylib.h"

#include "Board/Board.hpp"
#include "chess/Board.hpp"

#include <vector>
#include <utility>

class DragDropSystem
{
public:
    struct DragView
    {
        bool active;
        int source_index;
        int target_index;
        char piece;
        Vector2 world_pos;
        Vector2 grab_offset;
    };

    
    DragDropSystem();
    void Update(Board &board, const BoardView &view, const Camera2D &camera);
    DragView GetDragView() const;


private:
    struct DragState
    {
        bool active;
        int source_index;
        int target_index;
        char piece;
        Vector2 grab_offset;
        Vector2 last_mouse_world;
    };

    static bool WorldToIndex(Vector2 world, const BoardView &view, int &out_index);
    static Vector2 IndexToWorldCenter(int index, const BoardView &view);

    DragState drag_;
};
