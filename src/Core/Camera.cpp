#include "Camera.hpp"
#include "raylib.h"
#include "raymath.h"


Cam::Cam(float zoom)
{
    this->camera.zoom = zoom;
}

void Cam::update(bool isMouseOnBoard)
{
    this->Zoom();
	if (!isMouseOnBoard)
	{
		this->Move();
	}
}

void Cam::Move()
{

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f / this->camera.zoom);
        this->camera.target = Vector2Add(this->camera.target, delta);
    }
}

void Cam::Zoom()
{
    if (this->zoomMode == 0)
    {
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), this->camera);
            this->camera.offset = GetMousePosition();
            this->camera.target = mouseWorldPos;
            float scaleFactor = 1.0f + (0.25f * fabsf(wheel));
            if (wheel < 0)
                scaleFactor = 1.0f / scaleFactor;
            this->camera.zoom = Clamp(this->camera.zoom * scaleFactor, 0.125f, 64.0f);
        }
    }
    else
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), this->camera);
            this->camera.offset = GetMousePosition();
            this->camera.target = mouseWorldPos;
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            float deltaX = GetMouseDelta().x;
            float scaleFactor = 1.0f + (0.01f * fabsf(deltaX));
            if (deltaX < 0)
                scaleFactor = 1.0f / scaleFactor;
            this->camera.zoom = Clamp(this->camera.zoom * scaleFactor, 0.125f, 64.0f);
        }
    }
}
