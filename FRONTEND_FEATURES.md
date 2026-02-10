# Chess GUI Frontend Features

## Overview
Enhanced the chess GUI with comprehensive information display and controls to showcase all new engine features added during the refactoring phase.

## New UI Elements

### 1. **Right-Side Information Panel**
A persistent panel on the right side of the screen (340px wide) displays all game state information with organized sections:

#### Game Status Section
- **STATUS**: Displays current game state with color coding:
  - `RAYWHITE`: Normal play
  - `ORANGE`: Check
  - `RED`: Checkmate
  - `YELLOW`: Stalemate or Draw
- **To Move**: Shows which player's turn it is (WHITE or BLACK)

#### Position Metadata Section
- **Full Move**: Current full move number (increments after Black's move)
- **Half-Clock**: Half-move clock with color warning (turns red at 40+, orange at 20+)
  - Essential for the 50-move draw rule
- **Castling**: Shows available castling rights in standard notation
  - `KQkq`: Both sides can castle both ways
  - `K-`: White can only castle kingside
  - `-`: No castling available
- **En Passant**: Displays the en passant target square if available (e.g., "e3")

#### Draw Conditions Section
- Automatically detects and displays draw conditions:
  - Insufficient material (e.g., "King vs King", "King+Knight vs King")
  - Threefold repetition
  - Fifty-move rule (when half-clock reaches 50)
- Shows descriptive reason for the draw

#### Move History Section
- Displays the last 10 move pairs in standard chess notation
- Format: `1. e4 c5 2. Nf3 d6` etc.
- Current move is highlighted in `LIME` color
- Shows move numbers automatically

#### Controls Section
- **[U] Undo**: Shows availability and can be triggered with `U` key
  - `LIME`: Available
  - `DARKGRAY`: Unavailable
- **[R] Redo**: Shows availability and can be triggered with `R` key
  - `LIME`: Available
  - `DARKGRAY`: Unavailable

### 2. **Keyboard Controls**
- **U**: Undo the last move (if available)
- **R**: Redo the last undone move (if available)
- Visual feedback indicates when undo/redo are available

### 3. **Color-Coded Status Display**
Dynamic color coding helps users quickly understand game state:
- `ORANGE`: Check (king under attack)
- `RED`: Checkmate (game over, current player lost)
- `YELLOW`: Stalemate or Draw condition
- `RAYWHITE`: Normal play
- `LIME`: Current or available action
- `DARKGRAY`: Unavailable action

## Integration with Engine Features

### Undo/Redo System
The UI now exposes the complete undo/redo history via:
- `GameController::Undo()` and `GameController::Redo()` methods
- `GameController::CanUndo()` and `GameController::CanRedo()` checks
- Users can navigate entire game history with keyboard shortcuts

### Move History Display
Leverages new `GameController::GetMoveHistory()` and `GameController::GetMoveNotation()` methods:
- Displays moves in human-readable SAN notation (e.g., "Nf3", "Qxe4")
- Shows complete move pairs (White move + Black response per line)
- Highlights the currently selected move in the history

### Position Metadata Access
Utilizes new GameController query methods:
- `GetFullMoveNumber()`: Full move counter
- `GetHalfMoveClock()`: Half-move clock for 50-move rule
- `CanWhiteCastleKingside/Queenside()` and equivalent for Black
- `GetEnPassantSquare()`: Optional en passant target square

### Draw Detection
Integrates with the new DrawDetector utility:
- `IsDraw()`: Checks if any draw condition is met
- `GetDrawReason()`: Returns descriptive string explaining the draw
- Automatically updates based on current position

### Game Status
Uses enhanced GameController status methods:
- `GetStatusString()`: Returns status as string ("Check", "Checkmate", etc.)
- `IsInCheck()`: King under attack
- `IsCheckmate()`: Game over, current player lost
- `IsStalemate()`: Game over, draw by stalemate
- `IsWhiteToMove()`: Determines which player's turn it is

## Implementation Details

### Panel Layout
- Panel positioned at `x = GetScreenWidth() - 340`
- Spans full screen height except bottom 20px margin
- Semi-transparent dark background with border for visibility
- Fixed line height of 18px for consistent text spacing

### Move History Display Logic
```cpp
// Display last 10 move pairs
// Move pairs formatted as: 1. e4 c5  2. Nf3 d6  etc.
// Current move highlighted in LIME color
```

### Overflow Prevention
- Maximum of 35 lines displayed to prevent overflow off-screen
- Automatically displays only the most recent moves if history exceeds display capacity
- Half-move clock color coding: 
  - Normal (LIGHTGRAY): 0-20
  - Warning (ORANGE): 21-40
  - Critical (RED): 41-50

## Benefits

1. **Complete Game State Visibility**: Users see all relevant position information at a glance
2. **Move History Navigation**: Easy access to see what moves were played
3. **Draw Condition Awareness**: Clear indication of why a draw occurred
4. **Undo/Redo Functionality**: Full game history exploration capability
5. **Player Assistance**: Color coding helps identify check/checkmate situations
6. **60-Move Rule Awareness**: Half-move clock warning helps prevent accidental draws

## Future Enhancement Opportunities

1. **Clickable Move Replay**: Click move history to navigate to specific positions
2. **Material Count Display**: Show piece value for each side
3. **Evaluation Display**: Show engine evaluation if AI added
4. **PGN Export**: Save game history in standard PGN format
5. **Move Variants**: Display alternative moves at each position
6. **Time Controls**: Add game timer if implementing time-controlled play
7. **FEN Display**: Show FEN string for current position (useful for analysis)
