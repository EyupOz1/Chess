#include "raylib.h"
#include <array>
#include <stdint.h>

/// @brief Has a seperate Setup function
class BoardRenderer
{
    Texture2D pieceTexture;
    Color cellLight, cellDark;

    Rectangle cellDimensions[64];

public:
    void setup(Texture2D pieceTexture, Color cellLight, Color cellDark);
    void drawBoard();
    void drawPieces(std::array<uint8_t, 64> pieces);
};