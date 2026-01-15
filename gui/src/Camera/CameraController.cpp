#include "CameraController.hpp"

CameraController CreateCameraController(int screen_width, int screen_height)
{
    CameraController controller = {};
    controller.camera.target = {0.0f, 0.0f};
    controller.camera.offset = {screen_width * 0.5f, screen_height * 0.5f};
    controller.camera.rotation = 0.0f;
    controller.camera.zoom = 1.0f;
    controller.is_dragging = false;
    return controller;
}

void UpdateCameraController(CameraController &controller)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
    {
        controller.is_dragging = true;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE))
    {
        controller.is_dragging = false;
    }

    if (controller.is_dragging)
    {
        Vector2 delta = GetMouseDelta();
        controller.camera.target.x -= delta.x / controller.camera.zoom;
        controller.camera.target.y -= delta.y / controller.camera.zoom;
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f)
    {
        Vector2 mouse_pos = GetMousePosition();
        Vector2 before = GetScreenToWorld2D(mouse_pos, controller.camera);
        controller.camera.zoom += wheel * 0.1f;
        if (controller.camera.zoom < 0.1f)
        {
            controller.camera.zoom = 0.1f;
        }
        if (controller.camera.zoom > 6.0f)
        {
            controller.camera.zoom = 6.0f;
        }
        Vector2 after = GetScreenToWorld2D(mouse_pos, controller.camera);
        controller.camera.target.x += (before.x - after.x);
        controller.camera.target.y += (before.y - after.y);
    }
}
