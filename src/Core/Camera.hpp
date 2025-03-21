#pragma once

#include "raylib.h"

class Cam
{
public:
    Camera2D camera;
    char zoomMode = 0;

    Cam(float zoom);
    void Move();
    void Zoom();

    void update(bool isMouseOnBoard);
};