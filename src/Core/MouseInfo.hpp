#include "raylib.h"
#include "Core/Camera.hpp"

/// @brief Needs to be updatet every frame. 
class MouseInfo
{
    public:

    Vector2 screenPos;
    Vector2 worldPos;
    bool isOnBoard;
    MouseInfo();
    void update(Cam cam);
};
