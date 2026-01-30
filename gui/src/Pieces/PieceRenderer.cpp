#include "PieceRenderer.hpp"

#include "raymath.h"

#include "chess/Utils.hpp"

#include <algorithm>

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
        return {35, 35, 35, 255};
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

    static void DrawPieceFallback(char piece, Vector2 center, float tileSize)
    {
        float radius = tileSize * 0.35f;
        Vector2 shadowOffset = {2.0f, 3.0f};
        DrawCircleV(Vector2Add(center, shadowOffset), radius * 1.02f, (Color){0, 0, 0, 60});

        Color base = GetPieceFill(piece);
        Color rim = Engine::is_upper(piece) ? (Color){220, 220, 220, 255} : (Color){55, 55, 55, 255};
        DrawCircleGradient(static_cast<int>(center.x), static_cast<int>(center.y), radius, base, rim);
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, (Color){0, 0, 0, 90});
        DrawPieceGlyph(piece, center, tileSize);
    }

    static void DrawPieceTexture(Texture2D *tex, Vector2 center, float tileSize)
    {
        if (tex == nullptr || tex->id == 0)
        {
            return;
        }

        float maxSize = tileSize * 0.9f;
        float scale = maxSize / static_cast<float>(std::max(tex->width, tex->height));
        float drawW = tex->width * scale;
        float drawH = tex->height * scale;

        Rectangle src = {0.0f, 0.0f, static_cast<float>(tex->width), static_cast<float>(tex->height)};
        Rectangle dst = {center.x, center.y, drawW, drawH};
        Vector2 origin = {drawW * 0.5f, drawH * 0.5f};
        DrawTexturePro(*tex, src, dst, origin, 0.0f, WHITE);
    }

    void DrawPieces(const Engine::Board &board, const GUI::Board &boardView, const DragDrop::DragDropView &dragView, ThemeAssets &themeAssets)
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
            Texture2D *tex = GetTextureForPiece(themeAssets, piece);
            if (tex)
            {
                DrawPieceTexture(tex, center, boardView.tileSize);
            }
            else
            {
                DrawPieceFallback(piece, center, boardView.tileSize);
            }
        }

        if (dragView.isDragging && dragView.selectedPiece != 0)
        {
            Vector2 center = Vector2Add(dragView.worldPos, dragView.grabOffset);
            Texture2D *tex = GetTextureForPiece(themeAssets, dragView.selectedPiece);
            if (tex)
            {
                DrawPieceTexture(tex, center, boardView.tileSize);
            }
            else
            {
                DrawPieceFallback(dragView.selectedPiece, center, boardView.tileSize);
            }
        }
    }

}
