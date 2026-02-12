#include "raylib.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include "AppState.hpp"
#include "Pieces/PieceRenderer.hpp"
#include "UI/UI.hpp"

#include <algorithm>

void PushLog(AppState &state, const std::string &message, float duration)
{
  state.logMessage = message;
  state.logDuration = duration;
  state.logTimer = duration;
}

void ApplyTheme(AppState &state, int newIndex)
{
  if (state.themes.empty())
    return;

  if (newIndex < 0)
    newIndex = static_cast<int>(state.themes.size()) - 1;
  if (newIndex >= static_cast<int>(state.themes.size()))
    newIndex = 0;

  state.themeIndex = newIndex;
  const GUI::ThemeInfo &info = state.themes[state.themeIndex];
  if (!GUI::LoadThemeAssets(info, state.themeAssets))
  {
    PushLog(state, "Theme load failed");
    return;
  }
  state.boardView.lightColor = info.lightSquare;
  state.boardView.darkColor = info.darkSquare;
}

static void UpdateDrawFrame(void *user_data)
{
  auto *state = static_cast<AppState *>(user_data);

  state->camera.Update();

  // Input handling
  if (IsKeyPressed(KEY_F))
    state->boardView.flipped = !state->boardView.flipped;
  if (IsKeyPressed(KEY_F11))
    ToggleFullscreen();

  Engine::PositionStatus status = state->game.Status();
  bool isGameOver = status == Engine::PositionStatus::Checkmate ||
                    status == Engine::PositionStatus::Stalemate;

  // Drag & drop update
  if (!state->promotionActive && !isGameOver)
    state->dragDrop.Update(state->game.Board(), state->boardView, state->camera.camera);

  GUI::DragDrop::DragDropView dragView = state->dragDrop.GetView();

  // Move processing
  if (!state->promotionActive && !isGameOver)
  {
    GUI::DragDrop::MoveRequest moveRequest;
    if (state->dragDrop.ConsumeMoveRequest(moveRequest))
    {
      Engine::MoveResult result = state->game.TryMove(moveRequest.sourceIndex, moveRequest.targetIndex);
      if (result.error == Engine::MoveError::PromotionRequired)
      {
        state->promotionActive = true;
        state->promotionSource = moveRequest.sourceIndex;
        state->promotionTarget = moveRequest.targetIndex;
        state->dragDrop.ApplyMoveResult(false);
      }
      else
      {
        state->dragDrop.ApplyMoveResult(result.Ok());
        if (result.Ok())
          state->game.ClearCachedMoves();
        else
          PushLog(*state, "Move rejected");
      }
    }
  }

  // Undo/Redo
  if (IsKeyPressed(KEY_U) && state->game.CanUndo())
  {
    state->game.Undo();
    PushLog(*state, "Undo");
  }
  if (IsKeyPressed(KEY_R) && state->game.CanRedo())
  {
    state->game.Redo();
    PushLog(*state, "Redo");
  }

  // Rendering
  BeginDrawing();
  ClearBackground(RAYWHITE);

  BeginMode2D(state->camera.camera);
  state->boardView.DrawBoard();

  if (!state->promotionActive && !isGameOver && state->dragDrop.HasSelection())
  {
    state->game.UpdateCachedMoves(state->dragDrop.GetSelectedIndex(), state->dragDrop.GetSelectedPiece(), true);
    state->boardView.HighlightCells(state->game.CachedMoves());
  }
  else
  {
    state->game.UpdateCachedMoves(-1, 0, false);
  }

  GUI::DrawPieces(state->game.Board(), state->boardView, dragView, state->themeAssets);
  EndMode2D();

  state->logTimer = std::max(0.0f, state->logTimer - GetFrameTime());

  // UI panels
  GUI::DrawPromotionDialog(*state);
  GUI::DrawSidePanel(*state);
  GUI::DrawLogNotification(*state);

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

  GUI::UnloadThemeAssets(state.themeAssets);
  CloseWindow();
#endif
  return 0;
}
