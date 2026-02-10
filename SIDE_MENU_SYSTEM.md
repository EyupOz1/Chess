# Unified Side Menu System

## Overview
The chess GUI now features a clean, organized side menu on the right side of the screen that consolidates all game information, controls, and settings into a single tabbed interface.

## Menu Features

### Tab System
Two main tabs are available at the top of the side panel:

#### **Game Tab** (Default)
Shows all active game information and controls:
- **Status Section**: Current game state (Check, Checkmate, Stalemate, Draw)
- **Player Indicator**: Shows whose turn it is
- **Position Metadata**:
  - Current move number
  - 50-move rule clock (with color warning at 20+ and 40+)
  - Castling rights availability
  - En passant target square (if available)
- **Draw Conditions**: Displays if game is drawn and the reason
- **Move History**: Last 6 move pairs in standard notation (e.g., "1. e4 c5")
- **Controls**: Shows U/R key availability for undo/redo
- **Shortcuts**: Quick reference for keyboard controls

#### **Settings Tab**
Quick access to common settings:
- **Board Flip Toggle**: Easily flip the board with one click
- **Reset Position**: Load starting position with one click
- **Info Note**: References to the full settings menu (F1)

### Visual Design
- **Panel Dimensions**: 360px wide, spans full height
- **Semi-transparent background**: Dark gray (30, 30, 30) with 230 alpha for clean visibility
- **Tab Styling**:
  - Active tab: Bright blue (80, 120, 160)
  - Inactive tab: Dark gray (60, 60, 60)
  - Hover effect: Smooth color transitions
- **Text Hierarchy**: Section headers in skyblue, content in light gray
- **Color Coding**:
  - Orange: Check
  - Red: Checkmate
  - Yellow: Stalemate or Draw
  - Lime: Available actions or current move
  - Darkgray: Unavailable actions

### Keyboard Controls

| Key | Action |
|-----|--------|
| **M** | Toggle menu visibility |
| **U** | Undo last move |
| **R** | Redo last move |
| **F** | Flip board |
| **F11** | Fullscreen |
| **F1** | Open full settings menu |

### Interaction

#### Mouse Controls
- **Click Tab Headers**: Switch between Game and Settings tabs
- **Click Board Flip**: Toggle board orientation
- **Click Reset Position**: Load starting position

#### Touch/Responsive
- Tabs are clickable buttons with hover states
- Action buttons (Flip, Reset) highlight on hover
- All text is readable at default window size

## Benefits

### Reduced Clutter
- Removed scattered text from top-left corner
- All information organized into logical sections
- Clean board area without text overlays

### Better Organization
- **Game Tab**: Everything needed during play
- **Settings Tab**: Board configuration options
- **Full Menu (F1)**: Advanced settings (FEN, themes, etc.)

### Improved Usability
- Clear visual hierarchy with section headers
- Color coding for game states
- Quick-access buttons for common actions
- Keyboard shortcut reference always visible

### Professional Appearance
- Modern tabbed interface
- Consistent styling throughout
- Hover feedback for interactivity
- Organized information architecture

## Future Enhancements

1. **Menu Toggle**: Add ability to hide menu entirely (toggle with M key)
2. **Resizable Panel**: Allow user to adjust panel width
3. **Theme Selection**: Add theme selector to Settings tab
4. **Move Analysis**: Show evaluation/analysis if AI added
5. **Custom Colors**: Theme selection in Settings tab
6. **PGN Export**: Button to export game as PGN in Settings
7. **Time Control**: Show timers if implementing timed games
8. **Piece Count**: Display material advantage on game tab

## Technical Implementation

### Layout Calculations
```cpp
const int sideMenuWidth = 360;        // Fixed width
const int sideMenuX = GetScreenWidth() - sideMenuWidth;
const int sideMenuHeight = GetScreenHeight();

// Tabs at top
Rectangle gameTab = {sideMenuX + 5, sideMenuY + 5, (width - 10) / 2, 28};
Rectangle settingsTab = {sideMenuX + 5 + (width - 10) / 2, ...};

// Content starts below tabs
int contentStartY = sideMenuY + 45;
```

### Tab Detection
```cpp
bool mouseOnGameTab = CheckCollisionPointRec(mouse, gameTab);
bool mouseOnSettingsTab = CheckCollisionPointRec(mouse, settingsTab);

if (mouseOnGameTab && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
  state->showSettings = false;
}
```

### Overflow Prevention
- Maximum 28 lines for content area (prevents off-screen text)
- Move history limited to 6 pairs
- Automatic truncation of long text

## Comparison: Before vs After

### Before
- Top-left: Two lines of text controls
- Top-left: "F1 settings" indicator
- Right-side: Large panel with game info
- Cluttered appearance
- Multiple menus in different locations

### After
- Right-side: Single organized tabbed panel
- All information in one place
- Clean, professional appearance
- Tab-based navigation
- Clear visual hierarchy
- Better use of screen space
