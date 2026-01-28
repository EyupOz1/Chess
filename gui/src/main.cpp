#include "raylib.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include "Board/Board.hpp"
#include "Camera/CameraController.hpp"
#include "DragDrop/DragDrop.hpp"
#include "Game/GameController.hpp"
#include "Pieces/PieceRenderer.hpp"
#include "chess/Utils.hpp"

struct AppState
{
  const float tile_size = 64.0f;
  GUI::Board boardView;
  GUI::CameraController camera;
  GUI::DragDrop dragDrop;
  GUI::GameController game;
  bool promotionActive = false;
  int promotionSource = -1;
  int promotionTarget = -1;

  AppState(int screen_width, int screen_height)
      : boardView(tile_size, {0.0f, 0.0f}, false),
        camera(screen_width, screen_height)
  {
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

  Engine::PositionStatus status = state->game.Status();
  bool isGameOver = status == Engine::PositionStatus::Checkmate ||
                    status == Engine::PositionStatus::Stalemate;

  if (!state->promotionActive && !isGameOver)
  {
    state->dragDrop.Update(state->game.Board(), state->boardView, state->camera.camera);
  }

  GUI::DragDrop::DragDropView dragView = state->dragDrop.GetView();

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
        {
          state->game.ClearCachedMoves();
        }
      }
    }
  }

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

  GUI::DrawPieces(state->game.Board(), state->boardView, dragView);

  DrawCircle(0, 0, 10, BLACK);
  EndMode2D();

  if (state->promotionActive)
  {
    const float panelWidth = state->tile_size * 4.5f;
    const float panelHeight = state->tile_size * 1.2f;
    const float padding = 10.0f;
    float panelX = (GetScreenWidth() - panelWidth) * 0.5f;
    float panelY = GetScreenHeight() * 0.5f - panelHeight * 0.5f;

    DrawRectangle(static_cast<int>(panelX), static_cast<int>(panelY), static_cast<int>(panelWidth), static_cast<int>(panelHeight), {30, 30, 30, 220});
    DrawRectangleLines(static_cast<int>(panelX), static_cast<int>(panelY), static_cast<int>(panelWidth), static_cast<int>(panelHeight), RAYWHITE);

    char pawn = state->game.Board().PieceAt(state->promotionSource);
    bool isWhite = Engine::is_upper(pawn);
    const char options[4] = {'q', 'r', 'b', 'n'};
    float cellSize = state->tile_size;
    float startX = panelX + padding;
    float startY = panelY + (panelHeight - cellSize) * 0.5f;

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < 4; ++i)
    {
      float x = startX + i * (cellSize + padding * 0.5f);
      float y = startY;
      Rectangle cell = {x, y, cellSize, cellSize};
      bool hovered = CheckCollisionPointRec(mouse, cell);
      Color cellColor = hovered ? Color{80, 80, 80, 255} : Color{60, 60, 60, 255};
      DrawRectangleRec(cell, cellColor);
      DrawRectangleLines(static_cast<int>(cell.x), static_cast<int>(cell.y), static_cast<int>(cell.width), static_cast<int>(cell.height), RAYWHITE);

      char glyphChar = isWhite ? Engine::to_upper(options[i]) : Engine::to_lower(options[i]);
      char glyph[2] = {glyphChar, '\0'};
      int fontSize = static_cast<int>(cellSize * 0.5f);
      int textWidth = MeasureText(glyph, fontSize);
      DrawText(glyph,
               static_cast<int>(cell.x + cell.width * 0.5f - textWidth * 0.5f),
               static_cast<int>(cell.y + cell.height * 0.5f - fontSize * 0.5f),
               fontSize,
               RAYWHITE);

      if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      {
        Engine::MoveResult result = state->game.TryMove(state->promotionSource, state->promotionTarget, glyphChar);
        if (result.Ok())
        {
          state->promotionActive = false;
          state->promotionSource = -1;
          state->promotionTarget = -1;
          state->game.ClearCachedMoves();
          state->dragDrop.ApplyMoveResult(true);
        }
      }
    }
  }

  DrawText("LMB drag pieces; MMB/RMB pan; Wheel zoom; F flip; F11 fullscreen", 10, 10, 20, DARKGRAY);
  if (!state->game.Board().History().empty())
  {
    const Engine::Move &lastMove = state->game.Board().History().back();
    DrawText(TextFormat("Last Move: %c %i %i", lastMove.piece, lastMove.start, lastMove.end), 10, 30, 20, RED);
  }

  if (status == Engine::PositionStatus::Check)
  {
    DrawText("Check!", 10, 50, 22, MAROON);
  }
  else if (status == Engine::PositionStatus::Checkmate)
  {
    const char *winner = state->game.Board().IsWhiteTurn() ? "Black" : "White";
    DrawText(TextFormat("Checkmate! %s wins", winner), 10, 50, 22, MAROON);
  }
  else if (status == Engine::PositionStatus::Stalemate)
  {
    DrawText("Stalemate", 10, 50, 22, MAROON);
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
