#include "PieceLogic.h"
#include "Gamestate.h"
#include "Mods.h"
#include "GameModes.h"

#include <algorithm>
#include <random>

namespace {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, TETROMINO_COUNT - 1);
    std::uniform_int_distribution<int> rotDist(0, 3);
    std::uniform_int_distribution<int> garbageRNG(0, 1);   
}

void fillSevenBag()
{
    for (int i = 0; i < 7; i++)
    {
        sevenBag[i] = i;
    }

    std::shuffle(sevenBag.begin(), sevenBag.end(), rng);
}

void getNextPiece()
{
    if (isModOn(ModID::SevenBag))
    {
        if (sevenBagId >= 7) {
            fillSevenBag();
            sevenBagId = 0;
        }

        nextPiece.type = sevenBag[sevenBagId++];
    } else {
        nextPiece.type = dist(rng);
        if (nextPiece.type == current.type) nextPiece.type = dist(rng);
    }

    nextPiece.rotation = isModOn(ModID::RandomRotation) ? rotDist(rng) : 0;
}

bool checkCollision(int newX, int newY, int shape[4][4])
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (!shape[row][col])
                continue;
            
            int boardX = newX + col;
            int boardY = newY + row;

            // Board bounds
            if (boardX < 0 || boardX >= BOARD_WIDTH)
                return true;

            if (boardY >= BOARD_HEIGHT)
                return true;

            if (boardY >= 0 && board[boardY][boardX])
                return true;
        }
    }
    return false;
}

void spawnPiece(Piece piece)
{
    pieceActive = true;
    current.type = piece.type;
    current.rotation = piece.rotation;
    int spawnY = current.type == 0 ? -2 : -1;
    pieceCounts[current.type] += 1;

    if (checkCollision(spawnX, spawnY, tetrominoes[current.type][current.rotation])) {
        gameState = GameState::GameOver;
        return;
    }

    current.x = spawnX;
    current.y = spawnY;

    gravityAccumulator = 0.0f;

    if (!hasHeldPiece)
        getNextPiece();
}

void rotatePiece(int rotations)
{
    int newRotation = (current.rotation + rotations) % 4;

    if (newRotation < 0)
        newRotation += 4;

    if(!checkCollision(current.x, current.y, tetrominoes[current.type][newRotation]))
    {
        current.rotation = newRotation;
    }
}

void updateDAS(float deltaTime)
{
    if (inARE || inLineClear) return;

    if (dasDirection == 0 || !(holdingLeft || holdingRight))
    {
        dasActive = false;
        dasTimer = 0.0f;
        return;
    }

    dasTimer += deltaTime;

    if (!dasActive)
    {
        if (dasTimer >= DAS_INITIAL) {
            dasActive = true;
            dasTimer = 0.0f;
        }
        return;
    }

    while (dasTimer >= DAS_REPEAT) 
    {
        if(!checkCollision(current.x + dasDirection, current.y,
        tetrominoes[current.type][current.rotation]))
        {
            current.x += dasDirection;
        }
        dasTimer -= DAS_REPEAT;
    }
}

void updateLevel()
{
    int threshold1 = startLevel * 10 + 10;
    int threshold2 = std::max(100, startLevel * 10 - 50);
    int firstLevelUp = std::min(threshold1, threshold2);

    if (linesTotal < firstLevelUp)
    {
        level = startLevel;
    } else {
        int extraLines = linesTotal - firstLevelUp;
        level = startLevel + 1 + (extraLines / 10);
    }

    if (level < 29) {
        fallInterval = gravityFrames[level] / 60.0f;
    } else {
        fallInterval = 1.0f / 60.0f;
    }
}

void applyLineClear()
{
    for (int i = 0; i < linesToClear.size(); i++)
    {
        int y = linesToClear[i];

        for (int row = y; row > 0; row--)
        {
            for (int col = 0; col < BOARD_WIDTH; col++)
            {
                board[row][col] = board[row - 1][col];
            }
        }

        for (int col = 0; col < BOARD_WIDTH; col++)
            board[0][col] = 0;
    }
}

int detectLines() {
    linesToClear.clear();

    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        bool full = true;
        
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (board[y][x] == 0)
            {
                full = false;
                break;
            }
        }

        if (full) {
            linesToClear.push_back(y);
        }
    }

    return linesToClear.size();
}

float getARETime(int lockY)
{
    int base = 10 + (lockY / 4) * 2;
    return base / 60.0f;
}

void updateLineClear(float deltaTime)
{
    lineClearTimer += deltaTime;

    if (lineClearTimer >= CLEAR_STEP_TIME)
    {
        lineClearTimer -= CLEAR_STEP_TIME;
        clearStep++;

        if (clearStep >= 5)
        {
            applyLineClear();
            updateLevel();

            inLineClear = false;
            inARE = true;
            areTimer = getARETime(current.y);
        }
    }
}

void lockPiece()
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (tetrominoes[current.type][current.rotation][row][col])
            {
                int boardX = current.x + col;
                int boardY = current.y + row;

                if (boardY >= 0)
                    board[boardY][boardX] = current.type + 1;
            }
        }
    }

    pieceActive = false;

    int lines = detectLines();
    linesTotal += lines;

    if (lines > 0)
    {
        switch (lines) {
            case 1:
                score += 40 * (level + 1);
                break;
            case 2:
                score += 100 * (level + 1);
                break;
            case 3:
                score += 300 * (level + 1);
                break;
            case 4:
                score += 1200 * (level + 1);
                break;
        }

        inLineClear = true;
        lineClearTimer = 0.0f;
        clearStep = 0;
    } else {
        inARE = true;
        areTimer = getARETime(current.y);
    }

    hasHeldPiece = false;
}

void updateGravity(float deltaTime)
{
    gravityAccumulator += deltaTime;

    if (inARE || inLineClear) return;

    float interval = holdingDown ? 2.0f / 60.0f : fallInterval;

    if (holdingDown && !wasSoftDropping)
    {
        gravityAccumulator = std::min(gravityAccumulator, interval);
    }
    wasSoftDropping = holdingDown;

    while (gravityAccumulator >= interval)
    {
        if(!checkCollision(current.x, current.y + 1, 
            tetrominoes[current.type][current.rotation]))
        {
            current.y += 1;

            if (holdingDown) {
                score += 1;
            }
        } else {
            lockPiece();
            gravityAccumulator = 0.0f;
            return;
        }

        gravityAccumulator -= interval;
    }
}

void swapHeldPiece()
{
    if (inARE || inLineClear) return;

    if (hasHeldPiece) return;
    hasHeldPiece = true;

    if (heldPiece.type != -1) {
        tempPiece.type = heldPiece.type;
        tempPiece.rotation = heldPiece.rotation;

        heldPiece.type = current.type;
        heldPiece.rotation = isModOn(ModID::RandomRotation) ? current.rotation : 0;

        spawnPiece(tempPiece);
    } else {
        heldPiece.type = current.type;
        heldPiece.rotation = current.rotation;
        spawnPiece(nextPiece);
    }
}

int findFloorY()
{
    for (int i = current.y + 1; i < BOARD_HEIGHT; i++)
    {
        if (checkCollision(current.x, i, 
            tetrominoes[current.type][current.rotation])) 
        {
            return i - 1;
        }
    }
    return BOARD_HEIGHT;
}

void pushBoardUp(int y)
{
    for (int i = 0; i < y; i++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            board[i][x] = board[i + 1][x];
        }
    }
}

void setRowGarbage(int y)
{
    if (inARE || inLineClear || !linesToClear.empty()) return;

    if (checkCollision(current.x, current.y + 1, 
        tetrominoes[current.type][current.rotation]))
    {
        lockPiece();
        gravityAccumulator = 0.0f;
    }
    pushBoardUp(y);

    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        board[y][x] = garbageRNG(rng);
    }
}

void setBoardGarbage(int height)
{
    for (int y = BOARD_HEIGHT - height; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            board[y][x] = garbageRNG(rng);
        }
    }
}

void applyGameMode(GameMode mode)
{
    switch (mode) {
        case GameMode::NES:
            break;

        case GameMode::NESPlus:
            modValues[(int)ModID::ShowGhost] = true;
            modValues[(int)ModID::ShowGrid] = true;
            modValues[(int)ModID::HardDrop] = true;
            modValues[(int)ModID::SevenBag] = true;
            modValues[(int)ModID::EnableHold] = true;
            fillSevenBag();
            break;

        case GameMode::Garbage:
            setBoardGarbage(6);
            break;

        case GameMode::BurnDown:
            burnDownActive = true;
            setBoardGarbage(6);
            break;
    }
}

void resetGame()
{
    inARE = false;
    inLineClear = false;
    pieceActive = false;
    level = startLevel;
    linesTotal = 0;
    score = 0;
    updateLevel();
    countDown = 3.0f;
    burnDownActive = false;
    garbageTimer = 0.0f;
    scoreSaved = false;

    heldPiece.type = -1;
    for(int y = 0; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            board[y][x] = 0;
        }
    }

    for (int i = 0; i < TETROMINO_COUNT; i++)
    {
        pieceCounts[i] = 0;
    }

    applyGameMode(currentMode);
    fillSevenBag();
    getNextPiece();
    spawnPiece(nextPiece);
}