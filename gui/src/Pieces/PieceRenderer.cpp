#include "PieceRenderer.hpp"

#include "raymath.h"

#include "chess/Utils.hpp"

namespace GUI
{

    static Vector2 IndexToWorldTopLeft(int index, const GUI::Board &boardView)
    {
        int file = Engine::fileFromIndex(index, boardView.boardSize);
        int rank = Engine::rankFromIndex(index, boardView.boardSize);
        if (boardView.flipped)
        {
            file = (boardView.boardSize - 1) - file;
            rank = (boardView.boardSize - 1) - rank;
        }
        float x = boardView.origin.x + file * boardView.tileSize;
        int screenRank = (boardView.boardSize - 1) - rank;
        float y = boardView.origin.y + screenRank * boardView.tileSize;
        return {x, y};
    }

    static Vector2 IndexToWorldCenter(int index, const GUI::Board &boardView)
    {
        Vector2 topLeft = IndexToWorldTopLeft(index, boardView);
        return {topLeft.x + boardView.tileSize * 0.5f, topLeft.y + boardView.tileSize * 0.5f};
    }

    static Color GetPieceFill(char piece)
    {
        if (Engine::is_upper(piece))
        {
            return {245, 245, 245, 255};
        }
        return {40, 40, 40, 255};
    }

    static Color GetPieceTextColor(char piece)
    {
        if (Engine::is_upper(piece))
        {
            return {20, 20, 20, 255};
        }
        return {235, 235, 235, 255};
    }

    static void DrawPieceGlyph(char piece, Vector2 center, float tileSize)
    {
        char glyph[2] = {Engine::to_upper(piece), '\0'};
        int fontSize = static_cast<int>(tileSize * 0.5f);
        int textWidth = MeasureText(glyph, fontSize);
        DrawText(glyph,
                 static_cast<int>(center.x - textWidth * 0.5f),
                 static_cast<int>(center.y - fontSize * 0.5f),
                 fontSize,
                 GetPieceTextColor(piece));
    }

    static void DrawPiece(char piece, Vector2 center, float tileSize)
    {
        DrawCircleV(center, tileSize * 0.35f, GetPieceFill(piece));
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), tileSize * 0.35f, {0, 0, 0, 80});
        DrawPieceGlyph(piece, center, tileSize);
    }

    void DrawPieces(const Engine::Board &board, const GUI::Board &boardView, const DragDrop::DragDropView &dragView)
    {
        if (dragView.hasSelection && dragView.selectedIndex >= 0)
        {
            Vector2 topLeft = IndexToWorldTopLeft(dragView.selectedIndex, boardView);
            DrawRectangleLines(static_cast<int>(topLeft.x),
                               static_cast<int>(topLeft.y),
                               static_cast<int>(boardView.tileSize),
                               static_cast<int>(boardView.tileSize),
                               ORANGE);
        }

        if (dragView.isDragging && dragView.targetIndex >= 0)
        {
            Vector2 topLeft = IndexToWorldTopLeft(dragView.targetIndex, boardView);
            // Show the snapped target square while dragging.
            DrawRectangleLines(static_cast<int>(topLeft.x),
                               static_cast<int>(topLeft.y),
                               static_cast<int>(boardView.tileSize),
                               static_cast<int>(boardView.tileSize),
                               YELLOW);
        }

        int totalSquares = boardView.boardSize * boardView.boardSize;
        for (int index = 0; index < totalSquares; ++index)
        {
            char piece = board.PieceAt(index);
            if (piece == 0)
            {
                continue;
            }
            if (dragView.isDragging && index == dragView.selectedIndex)
            {
                continue;
            }
            Vector2 center = IndexToWorldCenter(index, boardView);
            DrawPiece(piece, center, boardView.tileSize);
        }

        if (dragView.isDragging && dragView.selectedPiece != 0)
        {
            Vector2 center = Vector2Add(dragView.worldPos, dragView.grabOffset);
            DrawPiece(dragView.selectedPiece, center, boardView.tileSize);
        }
    }

}
