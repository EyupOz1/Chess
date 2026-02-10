#pragma once

#include <string>

namespace Engine
{

    enum class MoveError
    {
        None,
        NoPiece,
        NotYourTurn,
        IllegalMove,
        PromotionRequired,
        InvalidFEN,
        InvalidPosition
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
        // Core move definition
        int from = -1;
        int to = -1;
        MoveType type = MoveType::Normal;

        // Piece information
        char pieceMoved = 0;          // The piece that moved
        char pieceCaptured = 0;       // Piece captured (0 if none)
        char promotion = 0;           // Promotion piece (0 if none)

        // State tracking (for undo/redo)
        bool castlingRightsChanged = false;
        int enPassantTargetAfter = -1;

        // Validation
        bool leavesKingInCheck = false;

        // Comparison
        bool operator==(const Move& other) const {
            return from == other.from && to == other.to && 
                   type == other.type && promotion == other.promotion;
        }

        // Human-readable representations
        std::string ToUCI() const;      // e2e4, e7e8q
        std::string ToSAN() const;      // e4, Nf3, O-O, Qxb7+
        std::string ToString() const;   // Descriptive format

        // Utilities
        bool IsCapture() const { return pieceCaptured != 0; }
        bool IsPromotion() const { return promotion != 0; }
        bool IsCastle() const { return type == MoveType::Castle; }
        bool IsEnPassant() const { return type == MoveType::EnPassant; }
    };

    struct MoveResult
    {
        MoveError error = MoveError::None;
        Move move = {};

        bool Ok() const { return error == MoveError::None; }
    };

}
