#include <vector>

class Player
{
    public:
    int selectedPiece = -1;
    std::vector<std::pair<int, int>> possibleMoves;
    void handlePieceSelection(int targetIndex);
};
