#include "Render.h"
#include <iomanip>
#include <sstream>

// --- Texture definitions ---
SDL_Texture* fontTexture;
SDL_Texture* levelColorTexture;
SDL_Texture* levelColorTextureSmall;
SDL_Texture* levelMenuTexture;
SDL_Texture* optionsMenuTexture;
SDL_Texture* gameBoardTexture;
SDL_Texture* gameBoardFlashTexture;
SDL_Texture* holdBoardTexture;
SDL_Texture* holdBoardFlashTexture;
SDL_Texture* burnBoardTexture;
SDL_Texture* burnBoardFlashTexture;

SDL_Texture* loadSprites(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_LoadBMP(path);
    SDL_Texture* spriteSheet = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return spriteSheet;
}

SDL_FRect getTileRect(int tileX, int tileY, int tileSize)
{
    return {
        (float)tileX * tileSize,
        (float)tileY * tileSize,
        (float)tileSize,
        (float)tileSize
    };
}

SDL_FRect getPaletteTile(int type, int level, int tileSize) {
    return {
        (float)(type * tileSize),
        (float)(level * tileSize),
        (float)tileSize,
        (float)tileSize
    };
}

int getCharIndex(char c) {
    if (c < 32 || c > 126) return 0;
    return c - 32;
}

void drawText(SDL_Renderer* renderer, SDL_Texture* fontTex,
    const std::string& text, int x, int y, int fontScale)
{
    int tileSize = 8;
    int textScale = fontScale * GAMESCALE / 4;
    int columns = 16;
    int posX = x == -1 ? (WINDOW_WIDTH - (text.length()*tileSize*textScale))/2 : x * GAMESCALE;
    int posY = y == -1 ? (WINDOW_HEIGHT - (tileSize*textScale))/2 : y * GAMESCALE;

    for (int i = 0; i < text.size(); i++)
    {
        int index = getCharIndex(text[i]);

        int tileX = index % columns;
        int tileY = index / columns;

        SDL_FRect src = getTileRect(tileX, tileY, tileSize);

        SDL_FRect dst = {
            (float)(posX + i * tileSize * textScale),
            (float)posY,
            (float)tileSize * textScale,
            (float)tileSize * textScale
        };

        SDL_RenderTexture(renderer, fontTex, &src, &dst);
    }
}

void drawCenteredText(
    SDL_Renderer* renderer, SDL_Texture* fontTex, const std::string& text,
    BoxName box, int fontScale)
{
    int tileSize = 8;
    int columns = 16;

    int textLength = text.size();
    int textScale = fontScale * GAMESCALE / 4;

    int x = boxPositions[(int)box][0] * GAMESCALE;
    int y = boxPositions[(int)box][1] * GAMESCALE;
    int w = boxPositions[(int)box][2] * GAMESCALE;
    int h = boxPositions[(int)box][3] * GAMESCALE;

    int posX = x + (w - (textLength * tileSize * textScale)) / 2;
    int posY = y + (h - (tileSize * textScale)) / 2;

    for (int i = 0; i < textLength; i++)
    {
        int index = getCharIndex(text[i]);

        int tileX = index % columns;
        int tileY = index / columns;

        SDL_FRect src = getTileRect(tileX, tileY, tileSize);

        SDL_FRect dst = {
            (float)(posX + i * tileSize * textScale),
            (float)posY,
            (float)tileSize * textScale,
            (float)tileSize * textScale
        };

        SDL_RenderTexture(renderer, fontTex, &src, &dst);
    }
}

int centerText(const std::string& text, BoxName box, int fontScale, bool horizontal)
{
    int tileSize = 8;

    int textLength = text.size();
    int textScale = fontScale * 2;
    int textWidth = textLength * textScale;
    int textHeight = tileSize * textScale;

    int x = boxPositions[(int)box][0];
    int y = boxPositions[(int)box][1];
    int w = boxPositions[(int)box][2];
    int h = boxPositions[(int)box][3];

    if (horizontal) {
        return x + (w - textWidth) / 2;
    } else {
        return y + (h - textHeight) / 2;
    }
}

std::string formatNumber(int value, int digits)
{
    std::ostringstream ss;
    ss << std::setw(digits)
        << std::setfill('0')
        << value;
    
    return ss.str();
}