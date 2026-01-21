#include "CameraController.hpp"


GUI::CameraController::CameraController(int screen_width, int screen_height)
{
    this->camera.target = {0.0f, 0.0f};
    this->camera.offset = {screen_width * 0.5f, screen_height * 0.5f};
    this->camera.rotation = 0.0f;
    this->camera.zoom = 1.0f;
    this->is_dragging = false;
}

void GUI::CameraController::Update()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
    {
        this->is_dragging = true;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE))
    {
        this->is_dragging = false;
    }

    if (this->is_dragging)
    {
        Vector2 delta = GetMouseDelta();
        this->camera.target.x -= delta.x / this->camera.zoom;
        this->camera.target.y -= delta.y / this->camera.zoom;
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f)
    {
        Vector2 mouse_pos = GetMousePosition();
        Vector2 before = GetScreenToWorld2D(mouse_pos, this->camera);
        this->camera.zoom += wheel * 0.1f;
        if (this->camera.zoom < 0.1f)
        {
            this->camera.zoom = 0.1f;
        }
        if (this->camera.zoom > 6.0f)
        {
            this->camera.zoom = 6.0f;
        }
        Vector2 after = GetScreenToWorld2D(mouse_pos, this->camera);
        this->camera.target.x += (before.x - after.x);
        this->camera.target.y += (before.y - after.y);
    }
}
