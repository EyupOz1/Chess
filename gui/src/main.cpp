#include "raylib.h"

#include "Board/Board.hpp"
#include "Camera/CameraController.hpp"
#include "DragDrop/DragDrop.hpp"
#include "Pieces/PieceRenderer.hpp"
#include "chess/Board.hpp"
#include "chess/MoveGen.hpp"

int main()
{
  const int screen_width = 800;
  const int screen_height = 450;
  InitWindow(screen_width, screen_height, "raylib: chess drag + snap");
  SetTargetFPS(60);

  const float tile_size = 64.0f;
  BoardView board_view = CreateBoardView(tile_size, {0.0f, 0.0f}, true);
  CameraController camera = CreateCameraController(screen_width, screen_height);
  DragDropSystem drag_drop;


  Board board;
  MoveGen moveGen;
  board.loadFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  while (!WindowShouldClose())
  {
    UpdateCameraController(camera);
    if (IsKeyPressed(KEY_F))
    {
      board_view.flipped = !board_view.flipped;
    }
    drag_drop.Update(board, board_view, camera.camera);
    DragDropSystem::DragView drag_view = drag_drop.GetDragView();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera.camera);
    DrawChessBoard(board_view);
    DrawPieces(board, board_view, drag_view);
    EndMode2D();

    DrawText("LMB drag pieces; MMB/RMB pan; Wheel zoom; F flip", 10, 10, 20, DARKGRAY);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
