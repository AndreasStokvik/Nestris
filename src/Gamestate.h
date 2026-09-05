#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <random>
#include <vector>

#include "Config.h"

extern int board[BOARD_HEIGHT][BOARD_WIDTH];

// Gravity
extern float fallInterval;
extern float fallTimer;
extern float gravityAccumulator;
extern int startLevel;

// ARE / line clear
extern bool inARE;
extern float areTimer;
extern bool inLineClear;
extern float lineClearTimer;
extern std::vector<int> linesToClear;
extern int clearStep;

// Countdown / garbage / burn mode
extern float countDown;
extern float garbageTimer;
extern bool burnDownActive;

// DAS / soft drop
extern float dasTimer;
extern int dasDirection;
extern bool holdingLeft;
extern bool holdingRight;
extern bool dasActive;
extern float softDropTimer;
extern bool holdingDown;
extern bool wasSoftDropping;

// Top level program state
extern GameState gameState;
extern MenuScreen currentMenu;

// Active piece / queue / hold
extern Piece current;
extern Piece nextPiece;
extern Piece heldPiece;
extern Piece tempPiece;
extern std::array<int, TETROMINO_COUNT> sevenBag;
extern int sevenBagId;
extern bool pieceActive;
extern bool hasHeldPiece;

// Score / level
extern int level;
extern int linesTotal;
extern int score;
extern int topScore;
extern bool scoreSaved;

extern int pieceCounts[TETROMINO_COUNT];


// Menu selection
extern int selectedMod;
extern int selectedMode;