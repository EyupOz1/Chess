#pragma once

#include "raylib.h"

#include "Board/Board.hpp"
#include "chess/Board.hpp"

namespace GUI
{

    struct DragDrop
    {
    public:
        struct DragDropView
        {
            bool isDragging;
            bool hasSelection;
            int selectedIndex;
            int targetIndex;
            char selectedPiece;
            Vector2 worldPos;
            Vector2 grabOffset;
        };

        DragDrop();
        void Update(Engine::Board &board, const GUI::Board &boardView, const Camera2D &camera);
        DragDropView GetView() const;
        int GetSelectedIndex() const;
        char GetSelectedPiece() const;
        bool HasSelection() const;

    private:
        struct State
        {
            bool isDragging;
            bool isMouseDown;
            bool hasSelection;
            int selectedIndex;
            int targetIndex;
            char selectedPiece;
            Vector2 grabOffset;
            Vector2 lastMouseWorld;
            Vector2 pressMouseWorld;
        };

        static void ClearSelection(State &state);
        static void SelectPiece(State &state, int index, char piece);
        static bool TryMove(Engine::Board &board, int sourceIndex, int targetIndex);
        static bool WorldToIndex(Vector2 world, const GUI::Board &boardView, int &outIndex);
        static Vector2 IndexToWorldCenter(int index, const GUI::Board &boardView);

        State dragDrop_;
    };

}
