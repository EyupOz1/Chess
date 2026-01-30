#pragma once

#include "raylib.h"

#include "Board/Board.hpp"
#include "DragDrop/DragDrop.hpp"
#include "Theme/Theme.hpp"
#include "chess/Board.hpp"

namespace GUI
{

    void DrawPieces(const Engine::Board &board, const GUI::Board &boardView, const DragDrop::DragDropView &dragView, ThemeAssets &themeAssets);

}
