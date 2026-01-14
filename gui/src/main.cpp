#include "raylib.h"
#include "chess/engine.hpp"

int main() {
  InitWindow(800, 450, chess::hello().c_str());
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Chess GUI (raylib)", 190, 200, 20, DARKGRAY);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
