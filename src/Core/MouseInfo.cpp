#include "Core/MouseInfo.hpp"
#include "Core/SETTING.hpp"

MouseInfo::MouseInfo()
{
}

void MouseInfo::update(Cam cam)
{
    this->screenPos = GetMousePosition();
    this->worldPos = GetScreenToWorld2D(this->screenPos, cam.camera);
    this->isOnBoard = CheckCollisionPointRec(this->worldPos, {0, 0, CELL_SIZE * 8, CELL_SIZE * 8});
}