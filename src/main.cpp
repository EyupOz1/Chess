#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "Defines.hpp"

#include "algorithm"

#include "Utils.hpp"

#include "Camera.hpp"
#include "Board/Board.hpp"
#include "MouseInfo.hpp"
#include "CommandBox.hpp"

Cam cam = Cam((Camera2D){.zoom = 1.0f});
Board board = Board();
MouseInfo mouseInfo;

int selectedCell = -1;
std::vector<int> selectedCellPossibleMoves;

// UI
CommandBox commandBox;

void setup()
{
	board.setup(FEN_START, LoadTexture("resources/pieces_tex.png"));
	mouseInfo.update(cam);
}

void update()
{
	mouseInfo.update(cam);
	cam.update(mouseInfo.isOnBoard);

	board.drawBoard();
	board.drawPieces();

	if (mouseInfo.isOnBoard)
	{
		board.highlightCell(mouseInfo.worldPos);

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			int targetIndex = board.getCellIndex(board.getCellPos(mouseInfo.worldPos));
			if (selectedCell == -1)
			{
				selectedCell = targetIndex;
				selectedCellPossibleMoves = board.getPossibleMoves(selectedCell);
			}
			else if (selectedCell == targetIndex)
			{
				selectedCell = -1;
				selectedCellPossibleMoves.clear();
			}
			else if (selectedCell != -1 &&
					 std::find(selectedCellPossibleMoves.begin(), selectedCellPossibleMoves.end(), targetIndex) != selectedCellPossibleMoves.end())
			{
				board.move(selectedCell, targetIndex);
				board.isBlacksTurn = !board.isBlacksTurn;
				selectedCell = -1;
				selectedCellPossibleMoves.clear();
			}
		}
	}

	board.highlightCell(selectedCell);
	for (auto possibleMove : selectedCellPossibleMoves)
	{
		board.highlightCell(possibleMove);
	}

	/*
std::vector<int> possibleMoves = board.getPossibleMoves(selectedCell, 0);

// Highlight Possible Moves of selected Cell
for (int possibleMove : possibleMoves)
board.highlightCell(possibleMove);

// Player Selecting
if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{

if (selectedCell == targetCellIndex)
currPlayer.unselectPiece();

else if (
	selectedCell != -1 &&
	std::find(possibleMoves.begin(), possibleMoves.end(), targetCellIndex) != possibleMoves.end())
	{
		board.move(players[board.isBlacksTurn].selectedCell, targetCellIndex);
		currPlayer.unselectPiece();
		board.isBlacksTurn = !board.isBlacksTurn;
	}
	else if (selectedCell == -1)
	currPlayer.selectPiece(targetCellIndex);
	else
	currPlayer.unselectPiece();
}
else
{
	// Unselect if click out of board
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		players[board.isBlacksTurn].unselectPiece();
	}
}
*/
}

void ui()
{
	if (IsKeyPressed(KEY_TAB))
	{
		commandBox.isVisible = !commandBox.isVisible;
	}

	commandBox.update(board);

	DrawCircleV(mouseInfo.screenPos, 4, DARKGRAY);
	DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", mouseInfo.screenPos.x, mouseInfo.screenPos.y),
			   Vector2Add(mouseInfo.screenPos, (Vector2){-44, -24}), 20, 2, DARKGRAY);

	DrawTextEx(GetFontDefault(), TextFormat("[%f, %f]", mouseInfo.worldPos.x, mouseInfo.worldPos.y), Vector2Add(mouseInfo.screenPos, (Vector2){-44, -44}), 20, 2, BLACK);
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
