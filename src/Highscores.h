#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "GameModes.h"

struct HighScoreEntry
{
    int score;
    int level;
    int lines;
    std::string date;
};

struct LeaderboardKey
{
    GameMode mode;
    uint32_t modMask;

    bool operator<(const LeaderboardKey& other) const
    {
        if (mode != other.mode)
            return mode < other.mode;

        return modMask < other.modMask;
    }
};

extern std::map<LeaderboardKey, std::vector<HighScoreEntry>> highScores;

uint32_t getModMask();
void addHighScore(int score, int level, int lines);
void saveHighScores();
void loadHighScores();
bool isHighScore(int score);
int getTopScore();
void updateHighScores();