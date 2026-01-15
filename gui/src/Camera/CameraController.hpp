#pragma once

#include "raylib.h"

struct CameraController
{
    Camera2D camera;
    bool is_dragging;
};

CameraController CreateCameraController(int screen_width, int screen_height);
void UpdateCameraController(CameraController &controller);
