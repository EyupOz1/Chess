#pragma once

#include "raylib.h"

#include <array>
#include <string>
#include <vector>

namespace GUI
{

    enum class PieceNaming
    {
        Standard,
        Illustrated
    };

    struct ThemeInfo
    {
        std::string name;
        std::string directory;
        Color lightSquare;
        Color darkSquare;
        bool pixelArt = false;
        PieceNaming naming = PieceNaming::Standard;
    };

    struct ThemeAssets
    {
        ThemeInfo info;
        std::array<Texture2D, 12> textures{};
        bool loaded = false;
    };

    std::string FindResRoot();
    std::vector<ThemeInfo> DiscoverThemes(const std::string &rootDir);
    bool LoadThemeAssets(const ThemeInfo &info, ThemeAssets &out);
    void UnloadThemeAssets(ThemeAssets &assets);
    Texture2D *GetTextureForPiece(ThemeAssets &assets, char piece);

}
