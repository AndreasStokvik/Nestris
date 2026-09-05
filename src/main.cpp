#include <SDL3/SDL.h>
#include <iostream>
#include <random>
#include <string>
#include <array>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <map>
#include <ctime>
#include <fstream>

#include "Config.h"
#include "Gamestate.h"
#include "Mods.h"
#include "GameModes.h"
#include "Highscores.h"
#include "Render.h"
#include "PieceLogic.h"

int fontScale = 4;

// --- GameState dependant handling ---
void updateMenu(float deltaTime){

}

void renderMenu(SDL_Renderer* renderer){    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    

    switch (currentMenu) {
        case MenuScreen::Main:
        {
            int fontScale = 4;

            SDL_FRect dst = {0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
            SDL_RenderTexture(renderer, levelMenuTexture, nullptr, &dst);
            
            //drawText(renderer, fontTexture, "PRESS ENTER TO START", -1, WINDOW_HEIGHT*0.30f, fontScale);
            drawCenteredText(renderer, fontTexture, "ENTER TO START", BoxName::MMTop, fontScale - 1);
            drawCenteredText(renderer, fontTexture, "LEVEL:", BoxName::MMLevelText, fontScale);
            drawCenteredText(renderer, fontTexture, std::to_string(startLevel), BoxName::MMLevelNum, fontScale);
            drawCenteredText(renderer, fontTexture, "GAME MODE", BoxName::MMGameMode, fontScale);
            

            // --- GAME MODE LIST ---
            fontScale = 3;
            int margins = 1 * GAMESCALE;
            int baseY = (boxPositions[(int)BoxName::MMGameModeList][1]) + margins;

            for (int i = 0; i < (int)GameMode::Count; i++)
            {
                std::string text = std::string(GameModeNames[i]);

                int y = baseY + (i) * (margins * fontScale);
                int x = (boxPositions[(int)BoxName::MMGameModeList][0]);

                if (i == (int)currentMode)
                {
                    text = "> " + text;
                } else {
                    x += (4*fontScale);
                }

                //if (i == (int)currentMode) SDL_SetTextureColorMod(fontTexture, 255, 255, 0); // Yellow for selected
                drawText(renderer, fontTexture, text, x, y, fontScale);
                SDL_SetTextureColorMod(fontTexture, 255, 255, 255);
            }

            drawText(renderer, fontTexture, "PRESS O FOR MORE OPTIONS", 
                (boxPositions[(int)BoxName::MMGameModeList][0]) + (2*fontScale), 
                baseY + (boxPositions[(int)BoxName::MMGameModeList][3] - margins * fontScale), 
                fontScale);
        }
        break;
        
        case MenuScreen::Options:
        {
            SDL_FRect dst = {0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
            SDL_RenderTexture(renderer, optionsMenuTexture, nullptr, &dst);

            int fontScale = 4;

            drawCenteredText(renderer, fontTexture, "OPTIONS", BoxName::MOTitle, fontScale);

            int baseY = boxPositions[(int)BoxName::MOContent][1] + fontScale;

            for (int i = 0; i < (int)ModID::Count; i++)
            {
                std::string text = std::string(ModNames[i]) + ": " + (modValues[i] ? "ON" : "OFF");

                int y = baseY + i * (4 * fontScale);
                int x = boxPositions[(int)BoxName::MOContent][0] + (4*fontScale);

                if (i == selectedMod)
                {
                    text = "> " + text;
                    x -= (4*fontScale);
                }

                if (modValues[i]) SDL_SetTextureColorMod(fontTexture, 0, 200, 255); // Cyan for Toggled ON
                if (i == selectedMod) SDL_SetTextureColorMod(fontTexture, 255, 255, 0); // Yellow for selected
                drawText(renderer, fontTexture, text, x, y, fontScale);
                SDL_SetTextureColorMod(fontTexture, 255, 255, 255);
            }

            drawText(renderer, fontTexture, "ESC TO GO BACK", -1, WINDOW_HEIGHT * 0.8f, fontScale - 1);
        }
        break;

    }
    
}

void handleMenuInput(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_KEY_DOWN) {
        
        switch (currentMenu) {
            case MenuScreen::Main:
                switch (event.key.key) {
                
                    case SDLK_RETURN:
                        resetGame();
                        gameState = GameState::Playing;
                        std::cout << "Game started\n";
                        break;
                    
                    case SDLK_LEFT:
                        if (startLevel > 0) startLevel--;
                        break;
                    
                    case SDLK_RIGHT:
                        startLevel++;
                        break;
                    
                    case SDLK_UP:
                        currentMode =
                            (GameMode)(((int)currentMode - 1 + (int)GameMode::Count)
                            % (int)GameMode::Count);
                        break;

                    case SDLK_DOWN:
                        currentMode =
                            (GameMode)(((int)currentMode + 1)
                            % (int)GameMode::Count);
                        break;

                    case SDLK_O:
                        currentMenu = MenuScreen::Options;
                        break;
                }
            break;

            case MenuScreen::Options:
                switch (event.key.key) {
                    
                        case SDLK_UP:
                            if (selectedMod > 0) selectedMod--;
                            else selectedMod = (int)ModID::Count - 1;
                            break;

                        case SDLK_DOWN:
                            if (selectedMod < (int)ModID::Count - 1) selectedMod++;
                            else selectedMod = 0;
                            break;

                        case SDLK_RETURN:
                        case SDLK_SPACE:
                            toggleMod((ModID)selectedMod);
                            break;

                        case SDLK_ESCAPE:
                            currentMenu = MenuScreen::Main;
                            break;
                    }
            break; 
        }
    }
}


void updateGame(float deltaTime) 
{
    // --- COUNT DOWN ---
    if (countDown > 0) {
        countDown -= deltaTime;
        return;
    }

    // --- ARE (Spawn Delay) ---
    if (inARE) {
        areTimer -= deltaTime;

        if (areTimer <= 0.0f) {
            inARE = false;
            spawnPiece(nextPiece);
        }
    }

    // --- LINE CLEARS ---
    if (inLineClear) {
        updateLineClear(deltaTime);
        return;
    }
    
    // --- Burndown Marathon ---
    if (burnDownActive) {
        if (inLineClear || !linesToClear.empty()) goto skip_garbage;
        if (level < 29) {
            if (garbageTimer > garbageLevels[level])
            {
                garbageTimer = 0.0f;
                setRowGarbage(BOARD_HEIGHT - 1);
            } else {
                garbageTimer += deltaTime;
            }
        } else {
            if (garbageTimer > 10) {
                garbageTimer = 0.0f;
                setRowGarbage(BOARD_HEIGHT - 1);
            } else {
                garbageTimer += deltaTime;
            }
        }
        skip_garbage:;
    }

    
    // --- DAS ---
    updateDAS(deltaTime);


    // --- GRAVITY ---
    updateGravity(deltaTime);
}

void renderGame(SDL_Renderer* renderer)
{
    // --- RENDERING ---
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_FRect dst = {0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    SDL_Texture* BG = gameBoardTexture;
    SDL_Texture* BGflash = gameBoardFlashTexture;

    int BGindex = 0;

    if (burnDownActive) {
        BGindex = 1;
    } else if (isModOn(ModID::EnableHold))
    {
        BGindex = 2;
    }

    switch (BGindex) {
        case 0:
            BG = gameBoardTexture;
            BGflash = gameBoardFlashTexture;
            break;
        case 1:
            BG = burnBoardTexture;
            BGflash = burnBoardFlashTexture;
            break;
        case 2:
            BG = holdBoardTexture;
            BGflash = holdBoardFlashTexture;
            break;
    }

    if (linesToClear.size() > 3 && inLineClear && clearStep % 2 == 0)
    {
        BG = BGflash;
    }
    SDL_RenderTexture(renderer, BG, nullptr, &dst);
    
    // Only draw board in GameState::Playing
    if (gameState == GameState::Playing || gameState == GameState::GameOver)
    {
        // Drawing board grid
        int gridcel = cellSize - 1 * GAMESCALE;
        if (isModOn(ModID::ShowGrid)) {
            for (int y = 0; y < BOARD_HEIGHT; y++) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    SDL_FRect grid;
                    grid.x = x * (gridcel + GAMESCALE) + BOARD_OFFSET_X;
                    grid.y = y * (gridcel + GAMESCALE) + BOARD_OFFSET_Y;
                    grid.w = gridcel;
                    grid.h = gridcel;
                    
                    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
                    SDL_RenderFillRect(renderer, &grid);
                }
            }
        }
    
        // Drawing locked pieces
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {

                int value = board[y][x];
                if (value == 0) continue;

                // Check if row is being cleared
                bool isClearingRow = false;
                if (inLineClear) {
                    for (int i = 0; i < linesToClear.size(); i++) {
                        if (linesToClear[i] == y) {
                            isClearingRow = true;
                            break;
                        }
                    }
                }

                // Animation
                if (inLineClear && isClearingRow)
                {
                    int left = 4 - clearStep;
                    int right = 5 + clearStep;

                    if (x >= left && x <= right) {
                        continue;
                    }
                }

                // Actual rendering
                SDL_FRect src = getPaletteTile(value - 1, level % 10, 8);

                SDL_FRect dst = {
                    (float)(x * cellSize + BOARD_OFFSET_X),
                    (float)(y * cellSize + BOARD_OFFSET_Y),
                    (float)cellSize,
                    (float)cellSize
                };

                SDL_RenderTexture(renderer, levelColorTexture, &src, &dst);
            }
        }
     
        // Drawing current piece
        if (pieceActive) {
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    if(tetrominoes[current.type][current.rotation][y][x]) {
                        if (current.y + y < 0) break;   // Don't render above board height
                        
                        SDL_FRect src = getPaletteTile(current.type, level  % 10, 8);

                        SDL_FRect dst = {
                            (float)((current.x + x) * cellSize + BOARD_OFFSET_X),
                            (float)((current.y + y) * cellSize + BOARD_OFFSET_Y),
                            (float)cellSize,
                            (float)cellSize
                        };

                        SDL_RenderTexture(renderer, levelColorTexture, &src, &dst);
                    }
                }
            }
        }

        // Drawing ghost piece
        int ghostOpacity = 100;
        if (pieceActive && isModOn(ModID::ShowGhost)) {
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    if(tetrominoes[current.type][current.rotation][y][x]) {
                        SDL_FRect src = getPaletteTile(current.type, level % 10, 8);

                        int ghostY = findFloorY(); 

                        SDL_FRect dst = {
                            (float)((current.x + x) * cellSize + BOARD_OFFSET_X),
                            (float)((ghostY + y) * cellSize + BOARD_OFFSET_Y),
                            (float)cellSize,
                            (float)cellSize
                        };


                        SDL_SetTextureAlphaMod(levelColorTexture, ghostOpacity);
                        SDL_RenderTexture(renderer, levelColorTexture, &src, &dst);
                    }
                }
            }
        }
        SDL_SetTextureAlphaMod(levelColorTexture, 255);

    }

    // Drawing next piece
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if(tetrominoes[nextPiece.type][nextPiece.rotation][y][x]) {
                SDL_FRect src = getPaletteTile(nextPiece.type, level  % 10, 8);
                
                int nextX = boxPositions[(int)BoxName::IGNext][0] * GAMESCALE;
                int nextY = boxPositions[(int)BoxName::IGNext][1] * GAMESCALE;
                
                if (nextPiece.type < 2) {
                    nextX = boxPositions[(int)BoxName::IGNextAlt][0] * GAMESCALE + cellSize;
                    nextY = boxPositions[(int)BoxName::IGNextAlt][1] * GAMESCALE + cellSize;
                    if (nextPiece.type == 0) nextY -= cellSize/2; 
                }
                
                
                SDL_FRect dst = {
                    (float)(nextX + (x - 1) * cellSize),
                    (float)(nextY + (y - 1) * cellSize),
                    (float)cellSize,
                    (float)cellSize
                };
                
                SDL_RenderTexture(renderer, levelColorTexture, &src, &dst);
            }
        }
    }

    // Drawing held piece
    if (heldPiece.type != -1) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if(tetrominoes[heldPiece.type][heldPiece.rotation][y][x]) {
                    SDL_FRect src = getPaletteTile(heldPiece.type, level  % 10, 8);
                    
                    int heldX = boxPositions[(int)BoxName::IGHold][0] * GAMESCALE;
                    int heldY = boxPositions[(int)BoxName::IGHold][1] * GAMESCALE;

                    if (heldPiece.type < 2) {
                        heldX = boxPositions[(int)BoxName::IGHoldAlt][0] * GAMESCALE + cellSize;
                        heldY = boxPositions[(int)BoxName::IGHoldAlt][1] * GAMESCALE + cellSize;
                        if (heldPiece.type == 0) heldY -= cellSize/2;
                    }

                    SDL_FRect dst = {
                        (float)(heldX + (x - 1) * cellSize),
                        (float)(heldY + (y - 1) * cellSize),
                        (float)cellSize,
                        (float)cellSize
                    };
                    
                    SDL_RenderTexture(renderer, levelColorTexture, &src, &dst);
                }
            }
        }
    }

    // Drawing Statistics pannel
    fontScale = 3;
    int statTitleY = boxPositions[(int)BoxName::IGStatisticsTitle][1];
    if (isModOn(ModID::EnableHold)) {
        statTitleY += 2 * GAMESCALE;
    }
    drawText(renderer, fontTexture, "STATISTICS",
        boxPositions[(int)BoxName::IGStatisticsTitle][0],
        statTitleY,
        fontScale
    );

    int smallCellSize = 6 * GAMESCALE;

    for (int i = 0; i < TETROMINO_COUNT; i++)
    {
        int displayType = 0;

        switch (i)
        {
            case 0: 
                displayType = 5;
                break;
            case 1: 
                displayType = 2;
                break;
            case 2: 
                displayType = 6;
                break;
            case 3: 
                displayType = 1;
                break;
            case 4: 
                displayType = 4;
                break;
            case 5: 
                displayType = 3;
                break;
            case 6: 
                displayType = 0;
                break;
            
        }
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if(tetrominoes[displayType][0][y][x]) {
                    SDL_FRect src = getPaletteTile(displayType, level  % 10, 6);
                    
                    int nextX = boxPositions[(int)BoxName::IGStatistics][0] * GAMESCALE + 4 * GAMESCALE;
                    int nextY = (boxPositions[(int)BoxName::IGStatistics][1] + statYOffsets[i]) * GAMESCALE;
                    
                    if (displayType == 0) {
                        nextX = boxPositions[(int)BoxName::IGStatistics][0] * GAMESCALE + smallCellSize;
                        nextY = (boxPositions[(int)BoxName::IGStatistics][1] + statYOffsets[i]) * GAMESCALE + smallCellSize;
                        nextY -= smallCellSize * 2; 
                    }
                    
                    SDL_FRect dst = {
                        (float)(nextX + (x - 1) * smallCellSize),
                        (float)(nextY + (y - 1) * smallCellSize),
                        (float)smallCellSize,
                        (float)smallCellSize
                    };
                    
                    SDL_RenderTexture(renderer, levelColorTextureSmall, &src, &dst);
                }
            }
        }

        SDL_SetTextureColorMod(fontTexture, 255, 0, 0);
        drawText(renderer,fontTexture, formatNumber(pieceCounts[displayType], 3),
            boxPositions[(int)BoxName::IGStatisticsValues][0],
            boxPositions[(int)BoxName::IGStatisticsValues][1] + i * 4 * GAMESCALE,
            fontScale);
    }




    // Rendering text
    SDL_SetTextureColorMod(fontTexture, 255, 255, 255);
    fontScale = 4;

    // Count down, not yet compared to original
    if (gameState != GameState::Paused)
    {
        if (countDown > 2.0f) {
            drawCenteredText(renderer, fontTexture, "3", BoxName::STANDARDGameBoard, fontScale);
        } else if (countDown > 1.0f) {
            drawCenteredText(renderer, fontTexture, "2", BoxName::STANDARDGameBoard, fontScale);
        } else if (countDown > 0.0f) {
            drawCenteredText(renderer, fontTexture, "1", BoxName::STANDARDGameBoard, fontScale);
        }
    }

    // Burn-down timer
    if (burnDownActive)
    {
        if (level < 29) {
            drawCenteredText(
                renderer, fontTexture,
                formatNumber(garbageLevels[level] - garbageTimer, 2),
                BoxName::BurnTimer, fontScale
            );
        }
        else {
            drawCenteredText(
                renderer, fontTexture,
                formatNumber(10 - garbageTimer, 2),
                BoxName::BurnTimer, fontScale
            );
        }
    }

    // Type
    if (isModOn(ModID::EnableHold))
    {
        drawText(renderer, fontTexture, "HELD",
            boxPositions[(int)BoxName::IGHoldText][0],
            boxPositions[(int)BoxName::IGHoldText][1], fontScale);
    } else if (burnDownActive) {
        drawCenteredText(
                renderer, fontTexture,
                "BURNDOWN",
                BoxName::IGType, fontScale - 1
            );
    } else {
        drawText(renderer, fontTexture, "NORMAL",
            boxPositions[(int)BoxName::IGType][0],
            boxPositions[(int)BoxName::IGType][1], fontScale);
    }


    // Next
    drawText(renderer, fontTexture, "NEXT",
        boxPositions[(int)BoxName::IGNextText][0],
        boxPositions[(int)BoxName::IGNextText][1], fontScale);

    // Level
    int margin = 1 * GAMESCALE / 2;
    drawText(renderer, fontTexture, "LEVEL", 
        centerText("LEVEL", BoxName::IGLevel, fontScale, true), 
        boxPositions[(int)BoxName::IGLevel][1] + margin, fontScale);
    drawText(renderer, fontTexture, formatNumber(level, 2), 
        centerText(formatNumber(level, 2), BoxName::IGLevel, fontScale, true),
        boxPositions[(int)BoxName::IGLevel][1] + margin * 6, fontScale);

    // Total Lines
    drawCenteredText(
        renderer, fontTexture, "LINES-" + formatNumber(linesTotal, 3), 
        BoxName::IGLines, fontScale);

    // Score
    drawText(renderer, fontTexture, "TOP",
        boxPositions[(int)BoxName::IGTopScore][0],
        boxPositions[(int)BoxName::IGTopScore][1], fontScale);
    drawText(renderer, fontTexture, formatNumber(getTopScore(), 6),
        boxPositions[(int)BoxName::IGTopScore][0],
        boxPositions[(int)BoxName::IGTopScore][1] + fontScale * GAMESCALE/2, fontScale);

    drawText(renderer, fontTexture, "SCORE", 
        boxPositions[(int)BoxName::IGScore][0], 
        boxPositions[(int)BoxName::IGScore][1], fontScale);
    drawText(renderer, fontTexture, formatNumber(score, 6), 
        boxPositions[(int)BoxName::IGScore][0], 
        boxPositions[(int)BoxName::IGScore][1] + fontScale * GAMESCALE/2, fontScale);
}

void handleGameInput(const SDL_Event& event)
{
    // --- INPUTS ---
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
        switch (event.key.key) 
        {
            case SDLK_RETURN:
                gameState = GameState::Paused;
                break;

            case SDLK_ESCAPE:
                gameState = GameState::Menu;
                updateHighScores();
                break;

            case SDLK_R:
                resetGame();
                break;

            case SDLK_G:
                if (isModOn(ModID::EnableHold))
                    swapHeldPiece();
                break;
            
            case SDLK_LEFT:
                holdingLeft = true;
                dasDirection = -1;
                dasTimer = 0.0f;

                // Tap
                if(!checkCollision(current.x - 1, current.y, 
                    tetrominoes[current.type][current.rotation]))
                {
                    current.x -= 1;
                } else {
                    dasTimer = DAS_INITIAL;
                }
                break;

            case SDLK_RIGHT:
                holdingRight = true;
                dasDirection = +1;
                dasTimer = 0.0f;

                // Tap
                if(!checkCollision(current.x + 1, current.y, 
                    tetrominoes[current.type][current.rotation]))
                {
                    current.x += 1;
                } else {
                    dasTimer = DAS_INITIAL;
                }
                break;

            case SDLK_DOWN:
                if (!inARE && !inLineClear && countDown <= 0.0f) holdingDown = true;
                break;

            // Rotation
            case SDLK_F:    // Clockwise
                if (!inARE && !inLineClear) rotatePiece(1);
                break;
            case SDLK_S:    // 180 degrees
                if (!inARE && !inLineClear) rotatePiece(2);
                break;
            case SDLK_D:    // Counter-clockwise
                if (!inARE && !inLineClear) rotatePiece(3);
                break;

            case SDLK_SPACE:
                if (isModOn(ModID::HardDrop) && countDown <= 0.0f) 
                {
                    current.y = findFloorY();
                    lockPiece();
                }
                break;
        }
    }

    if (event.type == SDL_EVENT_KEY_UP) {
        switch (event.key.key)
        {
        case SDLK_LEFT:
            holdingLeft = false;
            if (holdingRight) {
                dasDirection = +1;
                dasTimer = 0.0f;
            } else {
                dasDirection = 0;
            }
            break;
        
        case SDLK_RIGHT:
            holdingRight = false;
            if (holdingLeft) {
                dasDirection = -1;
                dasTimer = 0.0f;
            } else {
                dasDirection = 0;
            }
            break;

        case SDLK_DOWN:
            holdingDown = false;
            break;
        }
    }
}


void updatePause(float deltaTime)
{

}

void renderPause(SDL_Renderer* renderer)
{
    int fontScale = 4;

    drawCenteredText(renderer, fontTexture, "PAUSED", BoxName::STANDARDGameBoard, fontScale);
}

void handlePauseInput(const SDL_Event& event) 
{

    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_RETURN:
                gameState = GameState::Playing;
                break;

            case SDLK_ESCAPE:
                updateHighScores();
                gameState = GameState::Menu;
                currentMenu = MenuScreen::Main;
                break;

            case SDLK_R:
                updateHighScores();
                resetGame();
                gameState = GameState::Playing;
                break;
        }
    }
}


void updateGameOver(float deltaTime)
{
    updateHighScores();
}

void renderGameOver(SDL_Renderer* renderer)
{

    int fontScale = 4;

        drawCenteredText(renderer, fontTexture, "GAME OVER", BoxName::STANDARDGameBoard, fontScale);

}

void handleGameOverInput(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
        switch (event.key.key) 
        {
            case SDLK_RETURN: 
            case SDLK_R:
            case SDLK_SPACE:
                resetGame();
                gameState = GameState::Playing;
                break;

            case SDLK_ESCAPE:
                gameState = GameState::Menu;
                break;
        }
    }
}



int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Nestris", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if(!window){
        std::cout << "Window creation failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    bool running = true;
    SDL_Event event;

    // Load Highscores
    loadHighScores();



    // TextSprite
    fontTexture = loadSprites("assets//textSprite.bmp", renderer);
    SDL_SetTextureScaleMode(fontTexture, SDL_SCALEMODE_NEAREST);

    // Tetrominoes 8x8
    levelColorTexture = loadSprites("assets//levelColorSprite8x8.bmp", renderer);
    SDL_SetTextureScaleMode(levelColorTexture, SDL_SCALEMODE_NEAREST);

    // Tetrominoes 5x5
    levelColorTextureSmall = loadSprites("assets//levelColorSprite5x5.bmp", renderer);
    SDL_SetTextureScaleMode(levelColorTextureSmall, SDL_SCALEMODE_NEAREST);

    // Mode Select
    levelMenuTexture = loadSprites("assets//ModeLevelSelect.bmp", renderer);
    SDL_SetTextureScaleMode(levelMenuTexture, SDL_SCALEMODE_NEAREST);

    // Options menu
    optionsMenuTexture = loadSprites("assets//OptionsScreen.bmp", renderer);
    SDL_SetTextureScaleMode(optionsMenuTexture, SDL_SCALEMODE_NEAREST);

    // Standard game board (+ flash)
    gameBoardTexture = loadSprites("assets//StandardGameBoard.bmp", renderer);
    SDL_SetTextureScaleMode(gameBoardTexture, SDL_SCALEMODE_NEAREST);
    gameBoardFlashTexture = loadSprites("assets//StandardGameBoardFlash.bmp", renderer);
    SDL_SetTextureScaleMode(gameBoardFlashTexture, SDL_SCALEMODE_NEAREST);

    // Hold game board (+ flash)
    holdBoardTexture = loadSprites("assets//HoldGameBoard.bmp", renderer);
    SDL_SetTextureScaleMode(holdBoardTexture, SDL_SCALEMODE_NEAREST);
    holdBoardFlashTexture = loadSprites("assets//HoldGameBoardFlash.bmp", renderer);
    SDL_SetTextureScaleMode(holdBoardFlashTexture, SDL_SCALEMODE_NEAREST);
    
    // Hold game board (+ flash)
    burnBoardTexture = loadSprites("assets//BurnGameBoard.bmp", renderer);
    SDL_SetTextureScaleMode(burnBoardTexture, SDL_SCALEMODE_NEAREST);
    burnBoardFlashTexture = loadSprites("assets//BurnGameBoardFlash.bmp", renderer);
    SDL_SetTextureScaleMode(burnBoardFlashTexture, SDL_SCALEMODE_NEAREST);

    
    
    // --- MAIN LOOP ---
    Uint64 lastTime = SDL_GetTicks();
    while (running) 
    {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        

        // --- EVENT POLLING LOOP ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            switch (gameState) {
                case GameState::Menu:
                    handleMenuInput(event);
                    break;

                case GameState::Playing:
                    handleGameInput(event);
                    break;

                case GameState::Paused:
                    handlePauseInput(event);
                    break;

                case GameState::GameOver:
                    handleGameOverInput(event);
                    break;
            }
        }


        // --- Update ---
        switch (gameState) {
            case GameState::Menu:
                updateMenu(deltaTime);
                break;

            case GameState::Playing:
                updateGame(deltaTime);
                break;

            case GameState::Paused:
                updatePause(deltaTime);
                break;

            case GameState::GameOver:
                updateGameOver(deltaTime);
                break;
        }

        // --- Rendering ---
        SDL_RenderClear(renderer);
        
        switch (gameState) {
            case GameState::Menu:
            renderMenu(renderer);
            break;

        case GameState::Playing:
            renderGame(renderer);
            break;

        case GameState::Paused:
            renderGame(renderer);       // draw game underneath
            renderPause(renderer);      // overlay
            break;

        case GameState::GameOver:
            renderGame(renderer);       // draw game underneath
            renderGameOver(renderer);   // overlay
            break;
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}



#ifdef _WIN32
#include <windows.h>

int main(int argc, char* argv[]); // forward declaration

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}
#endif