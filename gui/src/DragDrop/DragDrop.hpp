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

        struct MoveRequest
        {
            int sourceIndex;
            int targetIndex;
        };

        DragDrop();
        void Update(const Engine::Board &board, const GUI::Board &boardView, const Camera2D &camera);
        bool ConsumeMoveRequest(MoveRequest &outRequest);
        void ApplyMoveResult(bool moved);
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
            bool hasPendingMove;
            int selectedIndex;
            int targetIndex;
            char selectedPiece;
            Vector2 grabOffset;
            Vector2 lastMouseWorld;
            Vector2 pressMouseWorld;
            MoveRequest pendingMove;
            MoveRequest lastMoveAttempt;
        };

        static void ClearSelection(State &state);
        static void SelectPiece(State &state, int index, char piece);
        static bool WorldToIndex(Vector2 world, const GUI::Board &boardView, int &outIndex);
        static Vector2 IndexToWorldCenter(int index, const GUI::Board &boardView);

        State dragDrop_;
    };

}
