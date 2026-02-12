#pragma once

#include "Board/Board.hpp"
#include "Camera/CameraController.hpp"
#include "DragDrop/DragDrop.hpp"
#include "Game/GameController.hpp"
#include "Theme/Theme.hpp"

#include <string>
#include <vector>

struct AppState;
void ApplyTheme(AppState &state, int newIndex);
void PushLog(AppState &state, const std::string &message, float duration = 2.5f);

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
  bool fenFieldActive = false;
  std::string fenInput;
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
