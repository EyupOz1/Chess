#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "Camera.hpp"
#include "Board.hpp"
#include "Defines.hpp"

int main()
{
	const int screenWidth = 1920 / 2;
	const int screenHeight = 1080 / 2;
	
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "");
	
	Texture2D tex = LoadTexture("resources/pieces_tex.png");
	Cam cam = Cam((Camera2D){.zoom = 1.0f});
	Board board = Board("r1bk3r/p2pBpNp/n4n2/1p1NP2P/6P1/3P4/P1P1K3/q5b1");


	SetTargetFPS(60);

	while (!WindowShouldClose())
	{

		BeginDrawing();
		{
			ClearBackground(GRAY);

			cam.Move();
			cam.Zoom(0);

			BeginMode2D(cam.camera);

			board.DrawBoard();
			board.DrawPieces(tex);

			EndMode2D();

			DrawCircleV(GetMousePosition(), 4, DARKGRAY);
			DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()),
					   Vector2Add(GetMousePosition(), (Vector2){-44, -24}), 20, 2, DARKGRAY);

			Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), cam.camera);
			DrawTextEx(GetFontDefault(), TextFormat("[%f, %f]", mouseWorldPos.x, mouseWorldPos.y), Vector2Add(GetMousePosition(), (Vector2){-44, -44}), 20, 2, BLACK);
		}
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
