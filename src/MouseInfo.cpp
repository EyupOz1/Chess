#include "MouseInfo.hpp"
#include "Defines.hpp"

MouseInfo::MouseInfo()
{
}

void MouseInfo::update(Cam cam)
{
    this->mouseWindowPos = GetMousePosition();
    this->mouseWorldPos = GetScreenToWorld2D(this->mouseWindowPos, cam.camera);
    this->isMouseOnBoard = CheckCollisionPointRec(this->mouseWorldPos, {0, 0, CELL_SIZE * 8, CELL_SIZE * 8});
}