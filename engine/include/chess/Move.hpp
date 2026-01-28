#pragma once

namespace Engine
{

    enum class MoveError
    {
        None,
        NoPiece,
        NotYourTurn,
        IllegalMove,
        PromotionRequired
    };

    enum class MoveType
    {
        Normal,
        Castle,
        EnPassant,
        Promotion
    };

    struct Move
    {
        char piece = 0;
        int start = -1;
        int end = -1;
        char promotion = 0;
        MoveType type = MoveType::Normal;
    };

    struct MoveResult
    {
        MoveError error = MoveError::None;
        Move move = {};

        bool Ok() const { return error == MoveError::None; }
    };

}
