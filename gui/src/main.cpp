#include "raylib.h"

#include "Board/Board.hpp"
#include "Camera/CameraController.hpp"
#include "DragDrop/DragDrop.hpp"
#include "Pieces/PieceRenderer.hpp"
#include "chess/Board.hpp"
#include "chess/MoveGen.hpp"

#include <vector>

int main()
{
  const int screen_width = 800;
  const int screen_height = 450;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(screen_width, screen_height, "raylib: chess drag + snap");
  SetTargetFPS(60);

  const float tile_size = 64.0f;
  GUI::Board boardView(tile_size, {0.0f, 0.0f}, false);
  GUI::CameraController camera(screen_width, screen_height);
  GUI::DragDrop dragDrop;

  Engine::Board board;
  board.LoadFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  Engine::MoveGen moveGen;
  std::vector<Engine::Move> cachedMoves;
  int lastSelectedIndex = -1;
  char lastSelectedPiece = 0;
  bool lastHasSelection = false;

  while (!WindowShouldClose())
  {
    camera.Update();

    if (IsKeyPressed(KEY_F))
      boardView.flipped = !boardView.flipped;

    if (IsKeyPressed(KEY_F11))
      ToggleFullscreen();

    dragDrop.Update(board, boardView, camera.camera);
    GUI::DragDrop::DragDropView dragView = dragDrop.GetView();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera.camera);
    boardView.DrawBoard();

    if (dragDrop.HasSelection())
    {
      int selectedIndex = dragDrop.GetSelectedIndex();
      char selectedPiece = dragDrop.GetSelectedPiece();
      bool selectionChanged = !lastHasSelection ||
                              selectedIndex != lastSelectedIndex ||
                              selectedPiece != lastSelectedPiece;
      if (selectionChanged)
      {
        cachedMoves = moveGen.GetPseudoLegalMoves(board, selectedIndex);
      }
      boardView.HighlightCells(cachedMoves);
    }
    else
    {
      cachedMoves.clear();
    }
    GUI::DrawPieces(board, boardView, dragView);

    DrawCircle(0, 0, 10, BLACK);
    EndMode2D();

    lastHasSelection = dragDrop.HasSelection();
    lastSelectedIndex = dragDrop.GetSelectedIndex();
    lastSelectedPiece = dragDrop.GetSelectedPiece();

    DrawText("LMB drag pieces; MMB/RMB pan; Wheel zoom; F flip; F11 fullscreen", 10, 10, 20, DARKGRAY);
    if (!board.history.empty())
    {
      const Engine::Move &lastMove = board.history.back();
      DrawText(TextFormat("Last Move: %c %i %i", lastMove.piece, lastMove.start, lastMove.end), 10, 30, 20, RED);
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
