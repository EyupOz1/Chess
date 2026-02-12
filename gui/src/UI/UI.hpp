#pragma once

#include "raylib.h"
#include <string>

struct AppState;

namespace GUI
{

  // UI primitives
  bool DrawButtonUI(Rectangle rect, const char *label);
  void DrawTextFieldUI(Rectangle rect, const std::string &text, bool active);

  // Composite panels
  void DrawSidePanel(AppState &state);
  void DrawPromotionDialog(AppState &state);
  void DrawLogNotification(AppState &state);

}
