#include "Theme.hpp"

#include "raylib.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <string>

namespace GUI
{
    namespace fs = std::filesystem;

    static std::string ToLower(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c)
                       { return static_cast<char>(std::tolower(c)); });
        return value;
    }

    static int PieceIndex(char piece)
    {
        if (piece == 0)
        {
            return -1;
        }
        char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(piece)));
        int base = std::isupper(static_cast<unsigned char>(piece)) ? 0 : 6;
        switch (lower)
        {
        case 'p':
            return base + 0;
        case 'n':
            return base + 1;
        case 'b':
            return base + 2;
        case 'r':
            return base + 3;
        case 'q':
            return base + 4;
        case 'k':
            return base + 5;
        default:
            return -1;
        }
    }

    static Color PickLightColor(const std::string &nameLower)
    {
        if (nameLower.find("wood") != std::string::npos)
            return {234, 214, 178, 255};
        if (nameLower.find("ocean") != std::string::npos)
            return {220, 235, 245, 255};
        if (nameLower.find("sky") != std::string::npos)
            return {230, 241, 250, 255};
        if (nameLower.find("space") != std::string::npos)
            return {45, 48, 70, 255};
        if (nameLower.find("neon") != std::string::npos)
            return {50, 52, 80, 255};
        if (nameLower.find("vintage") != std::string::npos)
            return {232, 223, 208, 255};
        return {240, 238, 230, 255};
    }

    static Color PickDarkColor(const std::string &nameLower)
    {
        if (nameLower.find("wood") != std::string::npos)
            return {149, 111, 67, 255};
        if (nameLower.find("ocean") != std::string::npos)
            return {72, 120, 152, 255};
        if (nameLower.find("sky") != std::string::npos)
            return {104, 141, 196, 255};
        if (nameLower.find("space") != std::string::npos)
            return {20, 22, 40, 255};
        if (nameLower.find("neon") != std::string::npos)
            return {22, 24, 48, 255};
        if (nameLower.find("vintage") != std::string::npos)
            return {126, 106, 82, 255};
        return {94, 120, 86, 255};
    }

    static bool IsPixelArtName(const std::string &nameLower)
    {
        return nameLower.find("8_bit") != std::string::npos ||
               nameLower.find("8-bit") != std::string::npos ||
               nameLower.find("pixel") != std::string::npos;
    }

    static bool HasStandardPieces(const fs::path &dir)
    {
        return fs::exists(dir / "wp.png") && fs::exists(dir / "bp.png");
    }

    static bool HasIllustratedPieces(const fs::path &dir)
    {
        return fs::exists(dir / "chess-pawn-white.png") && fs::exists(dir / "chess-pawn-black.png");
    }

    std::string FindResRoot()
    {
#if defined(PLATFORM_WEB)
        return std::string("/res");
#endif

        std::array<fs::path, 6> candidates = {
            fs::path("gui") / "res",
            fs::path("..") / "gui" / "res",
            fs::path("..") / ".." / "gui" / "res",
            fs::path("..") / ".." / ".." / "gui" / "res",
            fs::path("res"),
            fs::path("..") / "res"};

        for (const fs::path &candidate : candidates)
        {
            if (fs::exists(candidate) && fs::is_directory(candidate))
            {
                return candidate.string();
            }
        }

        return (fs::path("gui") / "res").string();
    }

    std::vector<ThemeInfo> DiscoverThemes(const std::string &rootDir)
    {
        std::vector<ThemeInfo> themes;

        fs::path root(rootDir);
        if (!fs::exists(root) || !fs::is_directory(root))
        {
            return themes;
        }

        for (const fs::directory_entry &entry : fs::directory_iterator(root))
        {
            if (!entry.is_directory())
            {
                continue;
            }

            fs::path dir = entry.path();
            bool standard = HasStandardPieces(dir);
            bool illustrated = HasIllustratedPieces(dir);
            if (!standard && !illustrated)
            {
                continue;
            }

            ThemeInfo info;
            info.name = dir.filename().string();
            info.directory = dir.string();
            std::string nameLower = ToLower(info.name);
            info.pixelArt = IsPixelArtName(nameLower);
            info.lightSquare = PickLightColor(nameLower);
            info.darkSquare = PickDarkColor(nameLower);
            info.naming = illustrated ? PieceNaming::Illustrated : PieceNaming::Standard;
            themes.push_back(info);
        }

        std::sort(themes.begin(), themes.end(), [](const ThemeInfo &a, const ThemeInfo &b)
                  { return a.name < b.name; });

        return themes;
    }

    bool LoadThemeAssets(const ThemeInfo &info, ThemeAssets &out)
    {
        UnloadThemeAssets(out);
        out.info = info;

        static const std::array<const char *, 12> standardNames = {
            "wp.png", "wn.png", "wb.png", "wr.png", "wq.png", "wk.png",
            "bp.png", "bn.png", "bb.png", "br.png", "bq.png", "bk.png"};

        static const std::array<const char *, 12> illustratedNames = {
            "chess-pawn-white.png", "chess-knight-white.png", "chess-bishop-white.png",
            "chess-rook-white.png", "chess-queen-white.png", "chess-king-white.png",
            "chess-pawn-black.png", "chess-knight-black.png", "chess-bishop-black.png",
            "chess-rook-black.png", "chess-queen-black.png", "chess-king-black.png"};

        const auto &names = (info.naming == PieceNaming::Illustrated) ? illustratedNames : standardNames;

        for (size_t i = 0; i < names.size(); ++i)
        {
            fs::path path = fs::path(info.directory) / names[i];
            if (!fs::exists(path))
            {
                UnloadThemeAssets(out);
                return false;
            }
            out.textures[i] = LoadTexture(path.string().c_str());
            if (out.textures[i].id == 0)
            {
                UnloadThemeAssets(out);
                return false;
            }
            SetTextureFilter(out.textures[i], info.pixelArt ? TEXTURE_FILTER_POINT : TEXTURE_FILTER_BILINEAR);
        }

        out.loaded = true;
        return true;
    }

    void UnloadThemeAssets(ThemeAssets &assets)
    {
        for (Texture2D &tex : assets.textures)
        {
            if (tex.id != 0)
            {
                UnloadTexture(tex);
            }
        }

        assets.textures = {};
        assets.loaded = false;
    }

    Texture2D *GetTextureForPiece(ThemeAssets &assets, char piece)
    {
        if (!assets.loaded)
        {
            return nullptr;
        }

        int index = PieceIndex(piece);
        if (index < 0)
        {
            return nullptr;
        }

        return &assets.textures[static_cast<size_t>(index)];
    }

}
