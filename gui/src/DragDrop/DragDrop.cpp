#include "DragDrop.hpp"
#include "chess/Utils.hpp"

#include "raymath.h"

#include <cmath>

namespace GUI
{

    DragDrop::DragDrop()
    {
        this->dragDrop_ = {};
        this->dragDrop_.selectedIndex = -1;
        this->dragDrop_.targetIndex = -1;
    }

    void DragDrop::ClearSelection(State &state)
    {
        state.hasSelection = false;
        state.selectedIndex = -1;
        state.targetIndex = -1;
        state.selectedPiece = 0;
    }

    void DragDrop::SelectPiece(State &state, int index, char piece)
    {
        state.hasSelection = true;
        state.selectedIndex = index;
        state.targetIndex = index;
        state.selectedPiece = piece;
    }

    bool DragDrop::WorldToIndex(Vector2 world, const GUI::Board &boardView, int &outIndex)
    {
        float fx = (world.x - boardView.origin.x) / boardView.tileSize;
        float fy = (world.y - boardView.origin.y) / boardView.tileSize;
        int file = static_cast<int>(floorf(fx));
        int screenRank = static_cast<int>(floorf(fy));
        if (!Engine::isOnBoard(file, screenRank, boardView.boardSize))
        {
            return false;
        }
        int rank = (boardView.boardSize - 1) - screenRank;
        if (boardView.flipped)
        {
            file = (boardView.boardSize - 1) - file;
            rank = (boardView.boardSize - 1) - rank;
        }
        outIndex = Engine::indexFromFileRank(file, rank, boardView.boardSize);
        return true;
    }

    Vector2 DragDrop::IndexToWorldCenter(int index, const GUI::Board &boardView)
    {
        int file = Engine::fileFromIndex(index, boardView.boardSize);
        int rank = Engine::rankFromIndex(index, boardView.boardSize);
        if (boardView.flipped)
        {
            file = (boardView.boardSize - 1) - file;
            rank = (boardView.boardSize - 1) - rank;
        }
        int screenRank = (boardView.boardSize - 1) - rank;
        float x = boardView.origin.x + file * boardView.tileSize + boardView.tileSize * 0.5f;
        float y = boardView.origin.y + screenRank * boardView.tileSize + boardView.tileSize * 0.5f;
        return {x, y};
    }

    void DragDrop::Update(const Engine::Board &board, const GUI::Board &boardView, const Camera2D &camera)
    {
        Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
        this->dragDrop_.lastMouseWorld = mouseWorld;

        int hoverIndex = -1;
        bool isHovering = WorldToIndex(mouseWorld, boardView, hoverIndex);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            this->dragDrop_.isMouseDown = true;
            this->dragDrop_.pressMouseWorld = mouseWorld;

            if (!isHovering)
            {
                ClearSelection(this->dragDrop_);
            }
            else
            {
                char hoveredPiece = board.PieceAt(hoverIndex);
                bool isFriendly = Engine::isFriendlyPiece(board.IsWhiteTurn(), hoveredPiece);

                if (this->dragDrop_.hasSelection)
                {
                    if (hoverIndex == this->dragDrop_.selectedIndex)
                    {
                        this->dragDrop_.targetIndex = hoverIndex;
                    }
                    else if (isFriendly)
                    {
                        SelectPiece(this->dragDrop_, hoverIndex, hoveredPiece);
                    }
                    else
                    {
                        this->dragDrop_.targetIndex = hoverIndex;
                    }
                }
                else if (isFriendly)
                {
                    SelectPiece(this->dragDrop_, hoverIndex, hoveredPiece);
                }
            }
        }

        if (this->dragDrop_.isMouseDown && this->dragDrop_.hasSelection)
        {
            float dragThreshold = boardView.tileSize * 0.15f;
            Vector2 dragDelta = Vector2Subtract(mouseWorld, this->dragDrop_.pressMouseWorld);
            float dragDistance = Vector2LengthSqr(dragDelta);

            if (!this->dragDrop_.isDragging && dragDistance > dragThreshold * dragThreshold)
            {
                this->dragDrop_.isDragging = true;
                Vector2 center = IndexToWorldCenter(this->dragDrop_.selectedIndex, boardView);
                this->dragDrop_.grabOffset = Vector2Subtract(center, mouseWorld);
            }

            if (this->dragDrop_.isDragging)
            {
                this->dragDrop_.targetIndex = isHovering ? hoverIndex : -1;
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            this->dragDrop_.isMouseDown = false;

            if (this->dragDrop_.isDragging)
            {
                if (this->dragDrop_.targetIndex >= 0 && this->dragDrop_.targetIndex != this->dragDrop_.selectedIndex)
                {
                    this->dragDrop_.pendingMove = {this->dragDrop_.selectedIndex, this->dragDrop_.targetIndex};
                    this->dragDrop_.lastMoveAttempt = this->dragDrop_.pendingMove;
                    this->dragDrop_.hasPendingMove = true;
                }
                else
                {
                    this->dragDrop_.targetIndex = this->dragDrop_.selectedIndex;
                }
            }
            else if (this->dragDrop_.hasSelection && isHovering && hoverIndex != this->dragDrop_.selectedIndex)
            {
                this->dragDrop_.pendingMove = {this->dragDrop_.selectedIndex, hoverIndex};
                this->dragDrop_.lastMoveAttempt = this->dragDrop_.pendingMove;
                this->dragDrop_.hasPendingMove = true;
            }

            this->dragDrop_.isDragging = false;
        }
    }

    bool DragDrop::ConsumeMoveRequest(MoveRequest &outRequest)
    {
        if (!this->dragDrop_.hasPendingMove)
        {
            return false;
        }

        outRequest = this->dragDrop_.pendingMove;
        this->dragDrop_.hasPendingMove = false;
        return true;
    }

    void DragDrop::ApplyMoveResult(bool moved)
    {
        if (moved)
        {
            ClearSelection(this->dragDrop_);
            return;
        }

        this->dragDrop_.targetIndex = this->dragDrop_.selectedIndex;
    }

    DragDrop::DragDropView DragDrop::GetView() const
    {
        DragDropView view = {};
        view.isDragging = this->dragDrop_.isDragging;
        view.hasSelection = this->dragDrop_.hasSelection;
        view.selectedIndex = this->dragDrop_.selectedIndex;
        view.targetIndex = this->dragDrop_.targetIndex;
        view.selectedPiece = this->dragDrop_.selectedPiece;
        view.worldPos = this->dragDrop_.lastMouseWorld;
        view.grabOffset = this->dragDrop_.grabOffset;
        return view;
    }

    int DragDrop::GetSelectedIndex() const
    {
        return this->dragDrop_.selectedIndex;
    }

    char DragDrop::GetSelectedPiece() const
    {
        return this->dragDrop_.selectedPiece;
    }

    bool DragDrop::HasSelection() const
    {
        return this->dragDrop_.hasSelection;
    }

}
