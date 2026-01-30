#include "raylib.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include "Board/Board.hpp"
#include "Camera/CameraController.hpp"
#include "DragDrop/DragDrop.hpp"
#include "Game/GameController.hpp"
#include "Pieces/PieceRenderer.hpp"
#include "Theme/Theme.hpp"
#include "chess/Utils.hpp"

#include <algorithm>
#include <string>

struct AppState;
static void ApplyTheme(AppState &state, int newIndex);
static void PushLog(AppState &state, const std::string &message, float duration = 2.5f);

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
  bool showSettings = false;
  bool fenFieldActive = false;
  std::string fenInput;
  std::string statusMessage;
  float statusTimer = 0.0f;
  std::string logMessage;
  float logTimer = 0.0f;
  float logDuration = 2.5f;
  std::string resRoot;
  std::vector<GUI::ThemeInfo> themes;
  int themeIndex = 0;
  GUI::ThemeAssets themeAssets;

  AppState(int screen_width, int screen_height)
      : boardView(tile_size, {0.0f, 0.0f}, false),
        camera(screen_width, screen_height)
  {
    fenInput = game.ExportFen();
    resRoot = GUI::FindResRoot();
    themes = GUI::DiscoverThemes(resRoot);
    if (!themes.empty())
    {
      ApplyTheme(*this, 0);
    }
    else
    {
      logMessage = "No themes found in gui/res";
      logTimer = logDuration;
    }
  }
};

static void PushLog(AppState &state, const std::string &message, float duration)
{
  state.logMessage = message;
  state.logDuration = duration;
  state.logTimer = duration;
}

static void ApplyTheme(AppState &state, int newIndex)
{
  if (state.themes.empty())
  {
    return;
  }

  if (newIndex < 0)
  {
    newIndex = static_cast<int>(state.themes.size()) - 1;
  }
  if (newIndex >= static_cast<int>(state.themes.size()))
  {
    newIndex = 0;
  }

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

static bool IsPointInRect(Vector2 point, Rectangle rect)
{
  return CheckCollisionPointRec(point, rect);
}

static bool DrawButton(Rectangle rect, const char *label)
{
  Vector2 mouse = GetMousePosition();
  bool hovered = IsPointInRect(mouse, rect);
  Color base = hovered ? Color{70, 70, 70, 235} : Color{55, 55, 55, 235};
  DrawRectangleRec(rect, base);
  DrawRectangleLines((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, RAYWHITE);
  int fontSize = 18;
  int textWidth = MeasureText(label, fontSize);
  DrawText(label,
           (int)(rect.x + rect.width * 0.5f - textWidth * 0.5f),
           (int)(rect.y + rect.height * 0.5f - fontSize * 0.5f),
           fontSize,
           RAYWHITE);
  return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void DrawTextField(Rectangle rect, const std::string &text, bool active)
{
  Color base = active ? Color{35, 35, 35, 230} : Color{28, 28, 28, 230};
  DrawRectangleRec(rect, base);
  DrawRectangleLines((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, RAYWHITE);
  int fontSize = 18;
  DrawText(text.c_str(), (int)rect.x + 8, (int)(rect.y + rect.height * 0.5f - fontSize * 0.5f), fontSize, RAYWHITE);
}

static void UpdateDrawFrame(void *user_data)
{
  auto *state = static_cast<AppState *>(user_data);

  state->camera.Update();

  if (IsKeyPressed(KEY_F1))
    state->showSettings = !state->showSettings;
  if (state->showSettings && IsKeyPressed(KEY_ESCAPE))
    state->showSettings = false;

  if (IsKeyPressed(KEY_F))
    state->boardView.flipped = !state->boardView.flipped;

  if (IsKeyPressed(KEY_F11))
    ToggleFullscreen();

  Engine::PositionStatus status = state->game.Status();
  bool isGameOver = status == Engine::PositionStatus::Checkmate ||
                    status == Engine::PositionStatus::Stalemate;

  if (!state->promotionActive && !isGameOver && !state->showSettings)
  {
    state->dragDrop.Update(state->game.Board(), state->boardView, state->camera.camera);
  }

  GUI::DragDrop::DragDropView dragView = state->dragDrop.GetView();

  if (!state->promotionActive && !isGameOver && !state->showSettings)
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
        else
        {
          PushLog(*state, "Move rejected");
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

  GUI::DrawPieces(state->game.Board(), state->boardView, dragView, state->themeAssets);

  DrawCircle(0, 0, 10, BLACK);
  EndMode2D();

  state->statusTimer = std::max(0.0f, state->statusTimer - GetFrameTime());
  state->logTimer = std::max(0.0f, state->logTimer - GetFrameTime());

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
        else
        {
          PushLog(*state, "Promotion move rejected");
        }
      }
    }
  }

  DrawText("LMB drag pieces; MMB pan; Wheel zoom; F flip; F11 fullscreen", 10, 10, 20, DARKGRAY);
  DrawText("F1 settings", 10, 34, 18, DARKGRAY);
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

  if (state->logTimer > 0.0f && !state->logMessage.empty())
  {
    float alpha = state->logTimer / std::max(0.01f, state->logDuration);
    alpha = std::min(alpha, 1.0f);
    int fontSize = 18;
    int textWidth = MeasureText(state->logMessage.c_str(), fontSize);
    float padX = 10.0f;
    float padY = 6.0f;
    float boxW = textWidth + padX * 2.0f;
    float boxH = fontSize + padY * 2.0f;
    Rectangle box = {10.0f, 60.0f, boxW, boxH};
    Color bg = {20, 20, 20, static_cast<unsigned char>(200.0f * alpha)};
    Color fg = {245, 245, 245, static_cast<unsigned char>(255.0f * alpha)};
    DrawRectangleRec(box, bg);
    DrawRectangleLines((int)box.x, (int)box.y, (int)box.width, (int)box.height, fg);
    DrawText(state->logMessage.c_str(), (int)(box.x + padX), (int)(box.y + padY), fontSize, fg);
  }

  if (state->showSettings)
  {
    const float panelWidth = 640.0f;
    const float panelHeight = 280.0f;
    float panelX = (GetScreenWidth() - panelWidth) * 0.5f;
    float panelY = (GetScreenHeight() - panelHeight) * 0.5f;

    DrawRectangle((int)panelX, (int)panelY, (int)panelWidth, (int)panelHeight, (Color){20, 20, 20, 235});
    DrawRectangleLines((int)panelX, (int)panelY, (int)panelWidth, (int)panelHeight, RAYWHITE);
    DrawText("Settings", (int)panelX + 16, (int)panelY + 12, 24, RAYWHITE);

    float themeRowY = panelY + 48;
    DrawText("Theme", (int)panelX + 16, (int)themeRowY - 18, 18, RAYWHITE);

    Rectangle themePrev = {panelX + 16, themeRowY, 32, 28};
    Rectangle themeNext = {panelX + panelWidth - 48, themeRowY, 32, 28};
    Rectangle themeNameRect = {panelX + 56, themeRowY, panelWidth - 112, 28};

    if (state->themes.empty())
    {
      DrawText("No themes found in gui/res", (int)themeNameRect.x + 4, (int)themeNameRect.y + 4, 16, RAYWHITE);
    }
    else
    {
      if (DrawButton(themePrev, "<"))
      {
        ApplyTheme(*state, state->themeIndex - 1);
        state->statusMessage = "Theme changed";
        state->statusTimer = 2.0f;
      }
      if (DrawButton(themeNext, ">"))
      {
        ApplyTheme(*state, state->themeIndex + 1);
        state->statusMessage = "Theme changed";
        state->statusTimer = 2.0f;
      }

      const std::string &themeName = state->themes[state->themeIndex].name;
      int nameSize = 18;
      int nameWidth = MeasureText(themeName.c_str(), nameSize);
      DrawRectangleLines((int)themeNameRect.x, (int)themeNameRect.y, (int)themeNameRect.width, (int)themeNameRect.height, RAYWHITE);
      DrawText(themeName.c_str(),
               (int)(themeNameRect.x + themeNameRect.width * 0.5f - nameWidth * 0.5f),
               (int)(themeNameRect.y + themeNameRect.height * 0.5f - nameSize * 0.5f),
               nameSize,
               RAYWHITE);
    }

    Rectangle fenRect = {panelX + 16, panelY + 110, panelWidth - 32, 36};
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
      state->fenFieldActive = IsPointInRect(mouse, fenRect);
    }

    if (state->fenFieldActive)
    {
      int key = GetCharPressed();
      while (key > 0)
      {
        if (key >= 32 && key <= 126)
        {
          if (state->fenInput.size() < 200)
          {
            state->fenInput.push_back((char)key);
          }
        }
        key = GetCharPressed();
      }

      if (IsKeyPressed(KEY_BACKSPACE) && !state->fenInput.empty())
      {
        state->fenInput.pop_back();
      }
    }

    DrawText("FEN", (int)panelX + 16, (int)panelY + 90, 18, RAYWHITE);
    DrawTextField(fenRect, state->fenInput, state->fenFieldActive);

    Rectangle loadBtn = {panelX + 16, panelY + 160, 140, 36};
    Rectangle copyBtn = {panelX + 166, panelY + 160, 140, 36};
    Rectangle pasteBtn = {panelX + 316, panelY + 160, 140, 36};
    Rectangle currentBtn = {panelX + 466, panelY + 160, 140, 36};

    if (DrawButton(loadBtn, "Load FEN"))
    {
      if (state->game.LoadFen(state->fenInput))
      {
        state->dragDrop.ApplyMoveResult(true);
        state->statusMessage = "FEN loaded";
      }
      else
      {
        state->statusMessage = "Invalid FEN";
        PushLog(*state, "Invalid FEN");
      }
      state->statusTimer = 2.0f;
    }

    if (DrawButton(copyBtn, "Copy FEN"))
    {
      std::string current = state->game.ExportFen();
      SetClipboardText(current.c_str());
      state->fenInput = current;
      state->statusMessage = "FEN copied to clipboard";
      state->statusTimer = 2.0f;
    }

    if (DrawButton(pasteBtn, "Paste"))
    {
      const char *clip = GetClipboardText();
      if (clip != nullptr)
      {
        state->fenInput = clip;
        state->statusMessage = "FEN pasted";
        state->statusTimer = 2.0f;
      }
    }

    if (DrawButton(currentBtn, "Use Current"))
    {
      state->fenInput = state->game.ExportFen();
      state->statusMessage = "FEN updated";
      state->statusTimer = 2.0f;
    }

    Rectangle resetBtn = {panelX + 16, panelY + 210, 140, 36};
    Rectangle flipBtn = {panelX + 166, panelY + 210, 140, 36};

    if (DrawButton(resetBtn, "Reset"))
    {
      state->game.LoadStartPosition();
      state->fenInput = state->game.ExportFen();
      state->statusMessage = "Start position loaded";
      state->statusTimer = 2.0f;
    }

    if (DrawButton(flipBtn, "Flip Board"))
    {
      state->boardView.flipped = !state->boardView.flipped;
    }

    DrawText("Esc/F1 to close", (int)panelX + 16, (int)panelY + panelHeight - 28, 16, GRAY);
    if (state->statusTimer > 0.0f && !state->statusMessage.empty())
    {
      DrawText(state->statusMessage.c_str(), (int)panelX + 16, (int)panelY + panelHeight - 52, 18, ORANGE);
    }
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

  GUI::UnloadThemeAssets(state.themeAssets);
  CloseWindow();
#endif
  return 0;
}
