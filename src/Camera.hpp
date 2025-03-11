#pragma once

#include "raylib.h"

class Cam
{
public:
    Camera2D camera;
    char zoomMode = 0;

    Cam(Camera2D camera);

    void Move();
    void Zoom();

    void update(bool isMouseOnBoard);
};