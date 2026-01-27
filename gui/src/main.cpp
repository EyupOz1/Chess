#include "raylib.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include "Board/Board.hpp"
#include "Camera/CameraController.hpp"
#include "DragDrop/DragDrop.hpp"
#include "Pieces/PieceRenderer.hpp"
#include "chess/Board.hpp"
#include "chess/MoveGen.hpp"

#include <vector>

struct AppState
{
  const float tile_size = 64.0f;
  GUI::Board boardView;
  GUI::CameraController camera;
  GUI::DragDrop dragDrop;
  Engine::Board board;
  Engine::MoveGen moveGen;
  std::vector<Engine::Move> cachedMoves;
  int lastSelectedIndex = -1;
  char lastSelectedPiece = 0;
  bool lastHasSelection = false;

  AppState(int screen_width, int screen_height)
      : boardView(tile_size, {0.0f, 0.0f}, false),
        camera(screen_width, screen_height)
  {
    board.LoadFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  }
};

static void UpdateDrawFrame(void *user_data)
{
  auto *state = static_cast<AppState *>(user_data);

  state->camera.Update();

  if (IsKeyPressed(KEY_F))
    state->boardView.flipped = !state->boardView.flipped;

  if (IsKeyPressed(KEY_F11))
    ToggleFullscreen();

  state->dragDrop.Update(state->board, state->boardView, state->camera.camera);
  GUI::DragDrop::DragDropView dragView = state->dragDrop.GetView();

  BeginDrawing();
  ClearBackground(RAYWHITE);

  BeginMode2D(state->camera.camera);
  state->boardView.DrawBoard();

  if (state->dragDrop.HasSelection())
  {
    int selectedIndex = state->dragDrop.GetSelectedIndex();
    char selectedPiece = state->dragDrop.GetSelectedPiece();
    bool selectionChanged = !state->lastHasSelection ||
                            selectedIndex != state->lastSelectedIndex ||
                            selectedPiece != state->lastSelectedPiece;
    if (selectionChanged)
    {
      state->cachedMoves = state->moveGen.GetPseudoLegalMoves(state->board, selectedIndex);
    }
    state->boardView.HighlightCells(state->cachedMoves);
  }
  else
  {
    state->cachedMoves.clear();
  }
  GUI::DrawPieces(state->board, state->boardView, dragView);

  DrawCircle(0, 0, 10, BLACK);
  EndMode2D();

  state->lastHasSelection = state->dragDrop.HasSelection();
  state->lastSelectedIndex = state->dragDrop.GetSelectedIndex();
  state->lastSelectedPiece = state->dragDrop.GetSelectedPiece();

  DrawText("LMB drag pieces; MMB/RMB pan; Wheel zoom; F flip; F11 fullscreen", 10, 10, 20, DARKGRAY);
  if (!state->board.history.empty())
  {
    const Engine::Move &lastMove = state->board.history.back();
    DrawText(TextFormat("Last Move: %c %i %i", lastMove.piece, lastMove.start, lastMove.end), 10, 30, 20, RED);
  }
  EndDrawing();
}

int main()
{
  const int screen_width = 800;
  const int screen_height = 450;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(screen_width, screen_height, "raylib: chess drag + snap");
  SetTargetFPS(60);

  AppState state(screen_width, screen_height);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop_arg(UpdateDrawFrame, &state, 0, true);
#else
  while (!WindowShouldClose())
  {
    UpdateDrawFrame(&state);
  }

  CloseWindow();
#endif
  return 0;
}
