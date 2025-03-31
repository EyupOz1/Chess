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

    void highlightCell(int cellIndex);
    void highlightCell(Vector2 targetCell);



    /// @param pos  Any World position 
    /// @return WorldPosition of the Cell
    Vector2 getCellAtPosition(Vector2 pos);
};