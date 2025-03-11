#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "Defines.hpp"

#include "Utils.hpp"

#include "Player.hpp"
#include "Camera.hpp"
#include "Board.hpp"
#include "MouseInfo.hpp"
#include "CommandBox.hpp"

Player players[] = {Player(), Player()};
Cam cam = Cam((Camera2D){.zoom = 1.0f});
Board board = Board();
MouseInfo mouseInfo;

// UI
CommandBox commandBox;

void setup()
{
	board.setup("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", LoadTexture("resources/pieces_tex.png"));
	mouseInfo.update(cam);
}

void update()
{
	mouseInfo.update(cam);
	cam.update(mouseInfo.isMouseOnBoard);

	board.drawBoard();
	board.drawPieces();

	if (mouseInfo.isMouseOnBoard)
	{
		Vector2 mouseCellCollisionInWorld = board.pointToCellPos(mouseInfo.mouseWorldPos);
		Vector2 cellPos = {mouseCellCollisionInWorld.x / CELL_SIZE, mouseCellCollisionInWorld.y / CELL_SIZE};
		if (mouseCellCollisionInWorld.x >= 0)
		{
			DrawRectangle(mouseCellCollisionInWorld.x, mouseCellCollisionInWorld.y, CELL_SIZE, CELL_SIZE, {0, 121, 241, 125});
		}
	}
}

void ui()
{
	if (IsKeyPressed(KEY_TAB))
	{
		commandBox.isVisible = !commandBox.isVisible;
	}

	commandBox.update(board);

	DrawCircleV(mouseInfo.mouseWindowPos, 4, DARKGRAY);
	DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", mouseInfo.mouseWindowPos.x, mouseInfo.mouseWindowPos.y),
			   Vector2Add(mouseInfo.mouseWindowPos, (Vector2){-44, -24}), 20, 2, DARKGRAY);

	DrawTextEx(GetFontDefault(), TextFormat("[%f, %f]", mouseInfo.mouseWorldPos.x, mouseInfo.mouseWorldPos.y), Vector2Add(mouseInfo.mouseWindowPos, (Vector2){-44, -44}), 20, 2, BLACK);
}

int main()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Chess");
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
