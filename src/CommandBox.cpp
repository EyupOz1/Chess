#include "CommandBox.hpp"
#include "raylib.h"
#include "raygui.h"
#include <iostream>
#include <cstdio>

void CommandBox::update(Board &board)
{
    if (this->isVisible)
    {
        if (GuiTextBox({0, static_cast<float>(GetScreenHeight() - 30), static_cast<float>(GetScreenWidth()), 30}, this->text, 255, true))
        {

            if (this->text[0] == 'm')
            {
                int begin, end;
                if (sscanf(this->text, "m%d-%d", &begin, &end) == 2) {
                    
                    board.move(begin, end);

                } else {
                    TraceLog(LOG_ERROR, "Error parsing");
                }
            }

            this->text[0] = '\0';
        }
    }
}