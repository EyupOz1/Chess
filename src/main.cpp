#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raymath.h"


#include "Board/BoardState.hpp"
#include "Board/BoardRenderer.hpp"

#include "Core/Setting.hpp"
#include "Core/Camera.hpp"
#include "Core/MouseInfo.hpp"

#include "Utils/Utils.hpp"

#include "Player.hpp"

Cam cam(1.0f);
MouseInfo mouseInfo;

Player player;

BoardState boardState;
BoardRenderer boardRenderer;

void setup()
{
    boardState.setup(FEN_startingPos);
    boardRenderer.setup(LoadTexture("resources/pieces_tex.png"), RAYWHITE, BROWN);
}

void update()
{
    mouseInfo.update(cam);

    cam.update(mouseInfo.isOnBoard);

    boardRenderer.drawBoard();
    boardRenderer.drawPieces(boardState.getBoard());

    boardRenderer.highlightCell(boardRenderer.getCellAtPosition(mouseInfo.worldPos));
    Vector2 selectedPiece = Vector2Scale(indexToCoords(player.selectedPiece), CELL_SIZE);
    
    boardRenderer.highlightCell(selectedPiece);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 cellWorldPos = boardRenderer.getCellAtPosition(mouseInfo.worldPos);
        cellWorldPos = {cellWorldPos.x / CELL_SIZE, cellWorldPos.y / CELL_SIZE};
        player.handlePieceSelection(coordsToIndex(cellWorldPos));
        TraceLog(2, "%i", player.selectedPiece);
    }
    
 
    
    
}

void ui()
{
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chess");
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_ALL);

    setup();

    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(GRAY);

        BeginMode2D(cam.camera);
        update();
        EndMode2D();
        ui();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
