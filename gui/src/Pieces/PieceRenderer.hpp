#pragma once

#include "raylib.h"

#include "Board/Board.hpp"
#include "DragDrop/DragDrop.hpp"
#include "chess/Board.hpp"

void DrawPieces(const Board &board, const BoardView &view, const DragDropSystem::DragView &drag);
