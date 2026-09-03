#include "Highscores.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <sstream>

#include "Gamestate.h"
#include "GameModes.h"
#include "Mods.h"

namespace
{
    std::string getCurrentDate()
    {
        time_t now = time(nullptr);

        tm localTime{};
        localtime_s(&localTime, &now);

        char buffer[32];

        strftime(
            buffer,
            sizeof(buffer),
            "%Y-%m-%d",
            &localTime
        );
        return buffer;
    }
}

std::map<LeaderboardKey, std::vector<HighScoreEntry>> highScores;

uint32_t getModMask()
{
    uint32_t mask = 0;

    for (int i = 0; i < (int)ModID::Count; i++)
    {
        if(modValues[i])
            mask |= (1 << i);
    }

    return mask;
}

void addHighScore(int score, int level, int lines)
{
    LeaderboardKey key{
        currentMode,
        getModMask()
    };

    HighScoreEntry entry{
        score, level, lines, getCurrentDate()
    };

    auto& board = highScores[key];

    board.push_back(entry);

    std::sort(
        board.begin(), board.end(),
        [](const HighScoreEntry& a, const HighScoreEntry& b)
        {
            return a.score > b.score;
        }
    );

    if (board.size() > 10)
        board.resize(10);
}

void saveHighScores()
{
    std::ofstream file("highscores.txt");

    if (!file)
        return;

    for (const auto& [key, board] : highScores)
    {
        file
            << (int)key.mode
            << " "
            << key.modMask
            << "\n";
        
        for (const auto& entry : board)
        {
            file
                << entry.score << " "
                << entry.level << " "
                << entry.lines << " "
                << entry.date << "\n";
        }

        file << "---\n";
    }
}

void loadHighScores()
{
    std::ifstream file("highscores.txt");

    if (!file)
        return;

    while (file)
    {
        int mode;
        uint32_t mask;

        if (!(file >> mode >> mask))
            break;

        LeaderboardKey key{
            (GameMode)mode,
            mask
        };

        std::vector<HighScoreEntry> board;

        while (true)
        {
            std::streampos pos = file.tellg();

            std::string line;
            std::getline(file >> std::ws, line);

            if (line == "---")
                break;

            std::istringstream iss(line);

            HighScoreEntry entry;

            iss
                >> entry.score
                >> entry.level
                >> entry.lines
                >> entry.date;

            board.push_back(entry);
        }

        highScores[key] = board;
    }
}

bool isHighScore(int score)
{
    LeaderboardKey key{
        currentMode,
        getModMask()
    };

    auto& board = highScores[key];

    if (board.size() < 10)
        return true;

    return score > board.back().score;
}

int getTopScore()
{
    LeaderboardKey key{
        currentMode,
        getModMask()
    };

    auto it = highScores.find(key);

    if (it == highScores.end() || it->second.empty())
        return 10000; // NES default highscore value

    return std::max(10000, it->second.front().score);
}

void updateHighScores()
{
    if (scoreSaved) return;

    if (score == 0) return;

    if (isHighScore(score))
    {
        addHighScore(score, level, linesTotal);
        saveHighScores();
        scoreSaved = true;
    }
}