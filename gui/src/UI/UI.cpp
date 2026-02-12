#include "UI.hpp"
#include "AppState.hpp"
#include "chess/Utils.hpp"

#include <algorithm>
#include <string>

static bool IsPointInRect(Vector2 point, Rectangle rect)
{
  return CheckCollisionPointRec(point, rect);
}

bool GUI::DrawButtonUI(Rectangle rect, const char *label)
{
  Vector2 mouse = GetMousePosition();
  bool hovered = IsPointInRect(mouse, rect);
  Color base = hovered ? Color{70, 70, 70, 235} : Color{55, 55, 55, 235};
  DrawRectangleRec(rect, base);
  DrawRectangleLines((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, RAYWHITE);
  int fontSize = 20;
  int textWidth = MeasureText(label, fontSize);
  DrawText(label,
           (int)(rect.x + rect.width * 0.5f - textWidth * 0.5f),
           (int)(rect.y + rect.height * 0.5f - fontSize * 0.5f),
           fontSize,
           RAYWHITE);
  return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void GUI::DrawTextFieldUI(Rectangle rect, const std::string &text, bool active)
{
  Color base = active ? Color{35, 35, 35, 230} : Color{28, 28, 28, 230};
  DrawRectangleRec(rect, base);
  DrawRectangleLines((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, RAYWHITE);
  int fontSize = 18;
  int padding = 8;
  BeginScissorMode((int)rect.x + padding, (int)rect.y, (int)rect.width - padding * 2, (int)rect.height);
  DrawText(text.c_str(), (int)rect.x + padding, (int)(rect.y + rect.height * 0.5f - fontSize * 0.5f), fontSize, RAYWHITE);
  EndScissorMode();
}

void GUI::DrawPromotionDialog(AppState &state)
{
  if (!state.promotionActive)
    return;

  const float panelWidth = state.tile_size * 4.5f;
  const float panelHeight = state.tile_size * 1.2f;
  const float padding = 10.0f;
  float panelX = (GetScreenWidth() - panelWidth) * 0.5f;
  float panelY = GetScreenHeight() * 0.5f - panelHeight * 0.5f;

  DrawRectangle(static_cast<int>(panelX), static_cast<int>(panelY), static_cast<int>(panelWidth), static_cast<int>(panelHeight), {30, 30, 30, 220});
  DrawRectangleLines(static_cast<int>(panelX), static_cast<int>(panelY), static_cast<int>(panelWidth), static_cast<int>(panelHeight), RAYWHITE);

  char pawn = state.game.Board().PieceAt(state.promotionSource);
  bool isWhite = Engine::is_upper(pawn);
  const char options[4] = {'q', 'r', 'b', 'n'};
  float cellSize = state.tile_size;
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
      Engine::MoveResult result = state.game.TryMove(state.promotionSource, state.promotionTarget, glyphChar);
      if (result.Ok())
      {
        state.promotionActive = false;
        state.promotionSource = -1;
        state.promotionTarget = -1;
        state.game.ClearCachedMoves();
        state.dragDrop.ApplyMoveResult(true);
      }
      else
      {
        PushLog(state, "Promotion move rejected");
      }
    }
  }
}

void GUI::DrawSidePanel(AppState &state)
{
  const int sideMenuWidth = 360;
  const int sideMenuX = GetScreenWidth() - sideMenuWidth;
  const int sideMenuY = 0;
  const int sideMenuHeight = GetScreenHeight();
  const int pad = 12;
  const int contentWidth = sideMenuWidth - pad * 2;

  // Panel background
  DrawRectangle(sideMenuX, sideMenuY, sideMenuWidth, sideMenuHeight, {30, 30, 30, 230});
  DrawRectangleLines(sideMenuX, sideMenuY, sideMenuWidth, sideMenuHeight, DARKGRAY);

  int x = sideMenuX + pad;
  int y = sideMenuY + pad;
  const int headerSize = 16;
  const int bodySize = 14;
  const int lineHeight = 22;
  Vector2 mouse = GetMousePosition();

  // === STATUS ===
  DrawText("STATUS", x, y, headerSize, SKYBLUE);
  y += lineHeight;

  std::string statusStr = state.game.GetStatusString();
  Color statusColor = RAYWHITE;
  if (state.game.IsInCheck())
    statusColor = ORANGE;
  if (state.game.IsCheckmate())
    statusColor = RED;
  if (state.game.IsStalemate())
    statusColor = YELLOW;
  if (state.game.IsDraw())
    statusColor = YELLOW;

  DrawText(statusStr.c_str(), x, y, bodySize, statusColor);
  y += lineHeight;

  const char *currentPlayer = state.game.IsWhiteToMove() ? "WHITE to move" : "BLACK to move";
  DrawText(currentPlayer, x, y, bodySize, RAYWHITE);
  y += lineHeight + 6;

  // === POSITION ===
  DrawText("POSITION", x, y, headerSize, SKYBLUE);
  y += lineHeight;

  DrawText(TextFormat("Move: %d", state.game.GetFullMoveNumber()), x, y, bodySize, LIGHTGRAY);
  y += lineHeight;

  int halfClock = state.game.GetHalfMoveClock();
  Color clockColor = (halfClock > 40) ? RED : (halfClock > 20) ? ORANGE
                                                                : LIGHTGRAY;
  DrawText(TextFormat("50-Move: %d/50", halfClock), x, y, bodySize, clockColor);
  y += lineHeight;

  std::string castlingStr = "";
  if (state.game.CanWhiteCastleKingside())
    castlingStr += "K";
  if (state.game.CanWhiteCastleQueenside())
    castlingStr += "Q";
  if (state.game.CanBlackCastleKingside())
    castlingStr += "k";
  if (state.game.CanBlackCastleQueenside())
    castlingStr += "q";
  if (castlingStr.empty())
    castlingStr = "-";
  DrawText(TextFormat("Castle: %s", castlingStr.c_str()), x, y, bodySize, LIGHTGRAY);
  y += lineHeight;

  if (state.game.GetEnPassantSquare() >= 0)
  {
    int file = state.game.GetEnPassantSquare() % 8;
    int rank = state.game.GetEnPassantSquare() / 8;
    DrawText(TextFormat("EP: %c%d", 'a' + file, rank + 1), x, y, bodySize, LIME);
    y += lineHeight;
  }

  if (state.game.IsDraw())
  {
    DrawText("DRAW", x, y, headerSize, YELLOW);
    y += lineHeight;
    std::string drawReason = state.game.GetDrawReason();
    DrawText(drawReason.c_str(), x, y, bodySize, YELLOW);
    y += lineHeight;
  }

  y += 6;

  // === MOVES ===
  DrawText("MOVES", x, y, headerSize, SKYBLUE);
  y += lineHeight;

  const auto &moveHistory = state.game.GetMoveHistory();
  int currentMoveIdx = state.game.GetCurrentMoveIndex();
  int maxMoves = 6;
  int startIdx = std::max(0, (int)moveHistory.size() - maxMoves);

  for (int i = startIdx; i < (int)moveHistory.size(); ++i)
  {
    if (i % 2 == 0)
    {
      int moveNum = i / 2 + 1;
      std::string displayStr = TextFormat("%d.", moveNum);

      std::string whiteMove = state.game.GetMoveNotation(i, true);
      displayStr += " " + whiteMove;

      if (i + 1 < (int)moveHistory.size())
      {
        std::string blackMove = state.game.GetMoveNotation(i + 1, true);
        displayStr += " " + blackMove;
      }

      Color moveColor = (i == currentMoveIdx || (i + 1 == currentMoveIdx)) ? LIME : LIGHTGRAY;
      DrawText(displayStr.c_str(), x, y, bodySize, moveColor);
      y += lineHeight;
    }
  }

  y += 6;

  // === FEN ===
  DrawText("FEN", x, y, headerSize, SKYBLUE);
  y += lineHeight;

  Rectangle fenRect = {(float)x, (float)y, (float)contentWidth, 36};
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
  {
    state.fenFieldActive = IsPointInRect(mouse, fenRect);
  }

  if (state.fenFieldActive)
  {
    int key = GetCharPressed();
    while (key > 0)
    {
      if (key >= 32 && key <= 126)
      {
        if (state.fenInput.size() < 200)
          state.fenInput.push_back((char)key);
      }
      key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && !state.fenInput.empty())
      state.fenInput.pop_back();
  }

  DrawTextFieldUI(fenRect, state.fenInput, state.fenFieldActive);
  y += 44;

  float btnWidth = (contentWidth - 8) / 3.0f;
  float btnHeight = 32;

  Rectangle loadBtn = {(float)x, (float)y, btnWidth, btnHeight};
  Rectangle copyBtn = {(float)x + btnWidth + 4, (float)y, btnWidth, btnHeight};
  Rectangle pasteBtn = {(float)x + (btnWidth + 4) * 2, (float)y, btnWidth, btnHeight};

  if (DrawButtonUI(loadBtn, "Load"))
  {
    if (state.game.LoadFen(state.fenInput))
    {
      state.dragDrop.ApplyMoveResult(true);
      PushLog(state, "FEN loaded");
    }
    else
    {
      PushLog(state, "Invalid FEN");
    }
  }

  if (DrawButtonUI(copyBtn, "Copy"))
  {
    std::string current = state.game.ExportFen();
    SetClipboardText(current.c_str());
    state.fenInput = current;
    PushLog(state, "FEN copied");
  }

  if (DrawButtonUI(pasteBtn, "Paste"))
  {
    const char *clip = GetClipboardText();
    if (clip != nullptr)
    {
      state.fenInput = clip;
      PushLog(state, "FEN pasted");
    }
  }

  y += (int)btnHeight + 10;

  // === THEME ===
  DrawText("THEME", x, y, headerSize, SKYBLUE);
  y += lineHeight;

  if (!state.themes.empty())
  {
    float prevW = 36;
    float nextW = 36;
    float nameW = contentWidth - prevW - nextW - 8;

    Rectangle themePrev = {(float)x, (float)y, prevW, 32};
    Rectangle themeNameRect = {(float)x + prevW + 4, (float)y, nameW, 32};
    Rectangle themeNext = {(float)x + prevW + nameW + 8, (float)y, nextW, 32};

    if (DrawButtonUI(themePrev, "<"))
      ApplyTheme(state, state.themeIndex - 1);
    if (DrawButtonUI(themeNext, ">"))
      ApplyTheme(state, state.themeIndex + 1);

    const std::string &themeName = state.themes[state.themeIndex].name;
    int nameSize = 18;
    int nameWidth = MeasureText(themeName.c_str(), nameSize);
    DrawRectangle((int)themeNameRect.x, (int)themeNameRect.y, (int)themeNameRect.width, (int)themeNameRect.height, {45, 45, 45, 235});
    DrawRectangleLines((int)themeNameRect.x, (int)themeNameRect.y, (int)themeNameRect.width, (int)themeNameRect.height, RAYWHITE);
    BeginScissorMode((int)themeNameRect.x, (int)themeNameRect.y, (int)themeNameRect.width, (int)themeNameRect.height);
    DrawText(themeName.c_str(),
             (int)(themeNameRect.x + themeNameRect.width * 0.5f - nameWidth * 0.5f),
             (int)(themeNameRect.y + themeNameRect.height * 0.5f - nameSize * 0.5f),
             nameSize, RAYWHITE);
    EndScissorMode();
  }
  else
  {
    DrawText("No themes found", x, y, bodySize, DARKGRAY);
  }

  y += 42;

  // === CONTROLS ===
  DrawText("CONTROLS", x, y, headerSize, SKYBLUE);
  y += lineHeight;

  float halfWidth = (contentWidth - 4) / 2.0f;
  Rectangle flipBtn = {(float)x, (float)y, halfWidth, 32};
  Rectangle resetBtn = {(float)x + halfWidth + 4, (float)y, halfWidth, 32};

  if (DrawButtonUI(flipBtn, "Flip Board"))
  {
    state.boardView.flipped = !state.boardView.flipped;
    PushLog(state, state.boardView.flipped ? "Board flipped" : "Board unflipped");
  }

  if (DrawButtonUI(resetBtn, "Reset"))
  {
    state.game.LoadStartPosition();
    state.fenInput = state.game.ExportFen();
    PushLog(state, "Position reset");
  }

  y += 42;

  Color undoColor = state.game.CanUndo() ? LIME : DARKGRAY;
  Color redoColor = state.game.CanRedo() ? LIME : DARKGRAY;
  DrawText("U - Undo", x, y, bodySize, undoColor);
  y += lineHeight;
  DrawText("R - Redo", x, y, bodySize, redoColor);
  y += lineHeight;
  DrawText("F - Flip board", x, y, bodySize, LIGHTGRAY);
  y += lineHeight;
  DrawText("F11 - Fullscreen", x, y, bodySize, LIGHTGRAY);
}

void GUI::DrawLogNotification(AppState &state)
{
  if (state.logTimer <= 0.0f || state.logMessage.empty())
    return;

  float alpha = state.logTimer / std::max(0.01f, state.logDuration);
  alpha = std::min(alpha, 1.0f);
  int fontSize = 20;
  int textWidth = MeasureText(state.logMessage.c_str(), fontSize);
  float padX = 12.0f;
  float padY = 8.0f;
  float boxW = textWidth + padX * 2.0f;
  float boxH = fontSize + padY * 2.0f;
  Rectangle box = {10.0f, 60.0f, boxW, boxH};
  Color bg = {20, 20, 20, static_cast<unsigned char>(200.0f * alpha)};
  Color fg = {245, 245, 245, static_cast<unsigned char>(255.0f * alpha)};
  DrawRectangleRec(box, bg);
  DrawRectangleLines((int)box.x, (int)box.y, (int)box.width, (int)box.height, fg);
  DrawText(state.logMessage.c_str(), (int)(box.x + padX), (int)(box.y + padY), fontSize, fg);
}
