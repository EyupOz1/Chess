#include "raylib.h"
#include "Camera.hpp"

class MouseInfo
{
    public:

    Vector2 mouseWindowPos;
    Vector2 mouseWorldPos;
    bool isMouseOnBoard;
    MouseInfo();
    void update(Cam cam);
};
