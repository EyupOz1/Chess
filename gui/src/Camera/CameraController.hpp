#pragma once

#include "raylib.h"

namespace GUI
{

    struct CameraController
    {
        Camera2D camera;
        bool is_dragging;

        CameraController(int screen_width, int screen_height);
        void Update();
    };


}
