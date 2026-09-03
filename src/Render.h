#pragma once

#include <SDL3/SDL.h>
#include <string>

#include "Config.h"

// --- Textures ---
extern SDL_Texture* fontTexture;
extern SDL_Texture* levelColorTexture;
extern SDL_Texture* levelColorTextureSmall;
extern SDL_Texture* levelMenuTexture;
extern SDL_Texture* optionsMenuTexture;
extern SDL_Texture* gameBoardTexture;
extern SDL_Texture* gameBoardFlashTexture;
extern SDL_Texture* holdBoardTexture;
extern SDL_Texture* holdBoardFlashTexture;
extern SDL_Texture* burnBoardTexture;
extern SDL_Texture* burnBoardFlashTexture;

// --- Sprite / Text helpers
SDL_Texture* loadSprites(const char* path, SDL_Renderer* renderer);
SDL_FRect getTileRect(int tileX, int tileY, int tileSize);
SDL_FRect getPaletteTile(int type, int level, int tileSize);
int getCharIndex(char c);

void drawText(SDL_Renderer* renderer, SDL_Texture* fontTex,
    const std::string& text, int x, int y, int fontScale);

void drawCenteredText(SDL_Renderer* renderer, SDL_Texture* fontTex,
    const std::string& text, BoxName box, int fontScale);

int centerText(const std::string& text, BoxName box, int fontScale, bool horizontal);

std::string formatNumber(int value, int digits);