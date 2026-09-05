#pragma once

#include "Config.h"
#include "GameModes.h"

void fillSevenBag();
void getNextPiece();
void spawnPiece(Piece piece);
void swapHeldPiece();

bool checkCollision(int newX, int newY, int shape[4][4]);
void rotatePiece(int rotations);
void updateDAS(float deltaTime);
int findFloorY();

void updateGravity(float deltaTime);
void lockPiece();

int detectLines();
void applyLineClear();
void updateLineClear(float deltaTime);
float getARETime(int lockY);
void updateLevel();

void pushBoardUp(int y);
void setRowGarbage(int y);
void setBoardGarbage(int height);

void applyGameMode(GameMode mode);
void resetGame();