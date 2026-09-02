#include "Gamestate.h"

int board[BOARD_HEIGHT][BOARD_WIDTH] = {0};

float fallInterval = 0.8f;
float fallTimer = 0.0f;
float gravityAccumulator = 0.0f;
int startLevel = 0;

bool inARE = false;
float areTimer = 0.0f;
bool inLineClear = false;
float lineClearTimer = 0.0f;
std::vector<int> linesToClear;
int clearStep = 0;

float countDown = 1.0f;
float garbageTimer = 0.0f;
bool burnDownActive = false;

float dasTimer = 0.0f;
int dasDirection = 0;
bool holdingLeft = false;
bool holdingRight = false;
bool dasActive = false;
float softDropTimer = 0.0f;
bool holdingDown = false;
bool wasSoftDropping = false;

GameState gameState = GameState::Menu;
MenuScreen currentMenu = MenuScreen::Main;

Piece current;
Piece nextPiece;
Piece heldPiece;
Piece tempPiece;
std::array<int, TETROMINO_COUNT> sevenBag = {};
int sevenBagId = 0;
bool pieceActive = true;
bool hasHeldPiece = false;
std::mt19937 rng(std::random_device{}());

int level = 0;
int linesTotal = 0;
int score = 0;
int topScore = 10000;
bool scoreSaved = false;

int selectedMod = 0;
int selectedMode = 0;