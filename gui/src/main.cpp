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

  // Undo/Redo controls
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

  // Toggle side menu with M key
  if (IsKeyPressed(KEY_M))
  {
    state->showSettings = !state->showSettings;
  }

  // === UNIFIED SIDE MENU PANEL (Right side) ===
  const int sideMenuWidth = 360;
  const int sideMenuX = GetScreenWidth() - sideMenuWidth;
  const int sideMenuY = 0;
  const int sideMenuHeight = GetScreenHeight();
  
  // Panel background
  DrawRectangle(sideMenuX, sideMenuY, sideMenuWidth, sideMenuHeight, {30, 30, 30, 230});
  DrawRectangleLines(sideMenuX, sideMenuY, sideMenuWidth, sideMenuHeight, DARKGRAY);
  
  int menuStartX = sideMenuX + 10;
  int menuStartY = sideMenuY + 10;
  
  // === TAB SELECTION ===
  Rectangle gameTab = {(float)sideMenuX + 5, (float)sideMenuY + 5, (sideMenuWidth - 10) / 2.0f, 28};
  Rectangle settingsTab = {(float)sideMenuX + 5 + (sideMenuWidth - 10) / 2.0f, (float)sideMenuY + 5, (sideMenuWidth - 10) / 2.0f, 28};
  
  Vector2 mouse = GetMousePosition();
  bool mouseOnGameTab = CheckCollisionPointRec(mouse, gameTab);
  bool mouseOnSettingsTab = CheckCollisionPointRec(mouse, settingsTab);
  
  // Game Tab button
  Color gameTabColor = state->showSettings ? Color{60, 60, 60, 255} : Color{80, 120, 160, 255};
  DrawRectangleRec(gameTab, gameTabColor);
  DrawRectangleLines((int)gameTab.x, (int)gameTab.y, (int)gameTab.width, (int)gameTab.height, LIGHTGRAY);
  DrawText("Game", menuStartX + 30, sideMenuY + 8, 14, RAYWHITE);
  
  if (mouseOnGameTab && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
  {
    state->showSettings = false;
  }
  
  // Settings Tab button
  Color settingsTabColor = state->showSettings ? Color{80, 120, 160, 255} : Color{60, 60, 60, 255};
  DrawRectangleRec(settingsTab, settingsTabColor);
  DrawRectangleLines((int)settingsTab.x, (int)settingsTab.y, (int)settingsTab.width, (int)settingsTab.height, LIGHTGRAY);
  DrawText("Settings", menuStartX + 150, sideMenuY + 8, 14, RAYWHITE);
  
  if (mouseOnSettingsTab && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
  {
    state->showSettings = true;
  }
  
  // Content area starts below tabs
  int contentStartY = sideMenuY + 45;
  int infoX = menuStartX;
  int infoY = contentStartY;
  int lineHeight = 16;
  int line = 0;
  
  if (!state->showSettings)
  {
    // === GAME INFO TAB ===
    DrawText("STATUS", infoX, infoY + (line++) * lineHeight, 12, SKYBLUE);
    
    std::string statusStr = state->game.GetStatusString();
    Color statusColor = RAYWHITE;
    if (state->game.IsInCheck())
      statusColor = ORANGE;
    if (state->game.IsCheckmate())
      statusColor = RED;
    if (state->game.IsStalemate())
      statusColor = YELLOW;
    if (state->game.IsDraw())
      statusColor = YELLOW;
    
    DrawText(TextFormat("%s", statusStr.c_str()), infoX, infoY + (line++) * lineHeight, 11, statusColor);
    
    const char *currentPlayer = state->game.IsWhiteToMove() ? "WHITE to move" : "BLACK to move";
    DrawText(currentPlayer, infoX, infoY + (line++) * lineHeight, 11, RAYWHITE);
    
    line++;  // Spacing
    
    // Position info
    DrawText("POSITION", infoX, infoY + (line++) * lineHeight, 12, SKYBLUE);
    
    DrawText(TextFormat("Move: %d", state->game.GetFullMoveNumber()), 
             infoX, infoY + (line++) * lineHeight, 11, LIGHTGRAY);
    
    int halfClock = state->game.GetHalfMoveClock();
    Color clockColor = (halfClock > 40) ? RED : (halfClock > 20) ? ORANGE : LIGHTGRAY;
    DrawText(TextFormat("50-Move: %d/50", halfClock), infoX, infoY + (line++) * lineHeight, 11, clockColor);
    
    // Castling rights
    std::string castlingStr = "";
    if (state->game.CanWhiteCastleKingside()) castlingStr += "K";
    if (state->game.CanWhiteCastleQueenside()) castlingStr += "Q";
    if (state->game.CanBlackCastleKingside()) castlingStr += "k";
    if (state->game.CanBlackCastleQueenside()) castlingStr += "q";
    if (castlingStr.empty()) castlingStr = "-";
    DrawText(TextFormat("Castle: %s", castlingStr.c_str()), infoX, infoY + (line++) * lineHeight, 11, LIGHTGRAY);
    
    // En passant
    if (state->game.GetEnPassantSquare() >= 0)
    {
      int file = state->game.GetEnPassantSquare() % 8;
      int rank = state->game.GetEnPassantSquare() / 8;
      DrawText(TextFormat("EP: %c%d", 'a' + file, rank + 1), infoX, infoY + (line++) * lineHeight, 11, LIME);
    }
    
    line++;  // Spacing
    
    // Draw conditions
    if (state->game.IsDraw())
    {
      DrawText("DRAW", infoX, infoY + (line++) * lineHeight, 12, YELLOW);
      std::string drawReason = state->game.GetDrawReason();
      DrawText(TextFormat("%s", drawReason.c_str()), infoX, infoY + (line++) * lineHeight, 10, YELLOW);
      line++;
    }
    
    // Move history
    DrawText("MOVES", infoX, infoY + (line++) * lineHeight, 12, SKYBLUE);
    
    const auto& moveHistory = state->game.GetMoveHistory();
    int currentMoveIdx = state->game.GetCurrentMoveIndex();
    int maxMoves = 6;
    int startIdx = std::max(0, (int)moveHistory.size() - maxMoves);
    
    for (int i = startIdx; i < (int)moveHistory.size(); ++i)
    {
      if (line > 28) break;
      
      if (i % 2 == 0)
      {
        int moveNum = i / 2 + 1;
        std::string displayStr = TextFormat("%d.", moveNum);
        
        if (i < (int)moveHistory.size())
        {
          std::string whiteMove = state->game.GetMoveNotation(i, true);
          displayStr += " " + whiteMove;
          
          if (i + 1 < (int)moveHistory.size())
          {
            std::string blackMove = state->game.GetMoveNotation(i + 1, true);
            displayStr += " " + blackMove;
          }
          
          DrawText(displayStr.c_str(), infoX, infoY + (line++) * lineHeight, 10, 
                   (i == currentMoveIdx || (i + 1 == currentMoveIdx)) ? LIME : LIGHTGRAY);
        }
      }
    }
    
    line++;  // Spacing
    
    // Undo/Redo controls
    DrawText("CONTROLS", infoX, infoY + (line++) * lineHeight, 12, SKYBLUE);
    Color undoColor = state->game.CanUndo() ? LIME : DARKGRAY;
    Color redoColor = state->game.CanRedo() ? LIME : DARKGRAY;
    DrawText(TextFormat("U: Undo"), infoX, infoY + (line++) * lineHeight, 10, undoColor);
    DrawText(TextFormat("R: Redo"), infoX, infoY + (line++) * lineHeight, 10, redoColor);
    
    line++;  // Spacing
    
    // Help text
    DrawText("SHORTCUTS", infoX, infoY + (line++) * lineHeight, 12, SKYBLUE);
    DrawText("M - Toggle menu", infoX, infoY + (line++) * lineHeight, 9, LIGHTGRAY);
    DrawText("F - Flip board", infoX, infoY + (line++) * lineHeight, 9, LIGHTGRAY);
    DrawText("F11 - Fullscreen", infoX, infoY + (line++) * lineHeight, 9, LIGHTGRAY);
  }
  else
  {
    // === SETTINGS TAB ===
    DrawText("THEMES", infoX, infoY + (line++) * lineHeight, 12, SKYBLUE);
    DrawText("(Use F1 to open", infoX, infoY + (line++) * lineHeight, 10, LIGHTGRAY);
    DrawText("full settings)", infoX, infoY + (line++) * lineHeight, 10, LIGHTGRAY);
    
    line++;
    DrawText("BOARD", infoX, infoY + (line++) * lineHeight, 12, SKYBLUE);
    
    // Quick flip button
    Rectangle flipButtonRect = {(float)infoX, (float)(infoY + (line) * lineHeight), (float)(sideMenuWidth - 20), 24};
    if (CheckCollisionPointRec(mouse, flipButtonRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
      state->boardView.flipped = !state->boardView.flipped;
      PushLog(*state, state->boardView.flipped ? "Board flipped" : "Board unflipped");
    }
    
    Color flipColor = CheckCollisionPointRec(mouse, flipButtonRect) ? Color{100, 140, 180, 255} : Color{60, 90, 120, 255};
    DrawRectangleRec(flipButtonRect, flipColor);
    DrawRectangleLines((int)flipButtonRect.x, (int)flipButtonRect.y, (int)flipButtonRect.width, (int)flipButtonRect.height, LIGHTGRAY);
    DrawText(state->boardView.flipped ? "Flip Board (ON)" : "Flip Board (OFF)", 
             infoX + 8, (int)flipButtonRect.y + 4, 12, RAYWHITE);
    line += 2;
    
    line++;
    DrawText("POSITION", infoX, infoY + (line++) * lineHeight, 12, SKYBLUE);
    
    // Quick reset button
    Rectangle resetButtonRect = {(float)infoX, (float)(infoY + (line) * lineHeight), (float)(sideMenuWidth - 20), 24};
    if (CheckCollisionPointRec(mouse, resetButtonRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
      state->game.LoadStartPosition();
      PushLog(*state, "Start position loaded");
    }
    
    Color resetColor = CheckCollisionPointRec(mouse, resetButtonRect) ? Color{100, 140, 180, 255} : Color{60, 90, 120, 255};
    DrawRectangleRec(resetButtonRect, resetColor);
    DrawRectangleLines((int)resetButtonRect.x, (int)resetButtonRect.y, (int)resetButtonRect.width, (int)resetButtonRect.height, LIGHTGRAY);
    DrawText("Reset Position", infoX + 8, (int)resetButtonRect.y + 4, 12, RAYWHITE);
    line += 2;
    
    line++;
    DrawText("INFO", infoX, infoY + (line++) * lineHeight, 12, SKYBLUE);
    DrawText("Press F1 for full", infoX, infoY + (line++) * lineHeight, 10, LIGHTGRAY);
    DrawText("settings menu", infoX, infoY + (line++) * lineHeight, 10, LIGHTGRAY);
    DrawText("(FEN, themes, etc)", infoX, infoY + (line++) * lineHeight, 10, LIGHTGRAY);
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
