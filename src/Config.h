#pragma once

constexpr int GAMESCALE = 4;
constexpr int WINDOW_WIDTH = 256 * GAMESCALE;
constexpr int WINDOW_HEIGHT = 224 * GAMESCALE;
constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;

enum class BoxName{
    MMTop,
    MMLevelText,
    MMLevelNum,
    MMGameMode,
    MMGameModeList,
    IGType,
    IGHoldText,
    IGHold,
    IGHoldAlt,
    IGLines,
    IGTopScore,
    IGScore,
    IGNextText,
    IGNext,
    IGNextAlt,
    IGLevel,
    IGStatisticsTitle,
    IGStatistics,
    IGStatisticsValues,
    MOTitle,
    MOContent,
    STANDARDGameBoard,
    BurnTimer,
    Count
};
constexpr int boxPositions[(int)BoxName::Count][4] = {
    // X, Y, W, H
    {29, 21, 102, 14},  // MMTop
    {60, 52, 72, 16},   // MMLevelText
    {156, 52, 32, 16},  // MMLevelNum
    {29, 93, 102, 14},  // MMGameMode
    {31, 126, 155, 75}, // MMGameModeList
    {24, 24, 48, 8},    // IGType
    {49, 16, 32, 32},   // IGHoldText
    {53, 32, 24, 24},   // IGHold
    {49, 24, 32, 32},   // IGHoldAlt
    {93, 13, 86, 14},   // IGLines
    {192, 24, 47, 16},  // IGTopScore
    {192, 48, 47, 16},  // IGScore
    {192, 96, 32, 32},  // IGNextText
    {196, 112, 24, 24}, // IGNext
    {192, 104, 32, 32}, // IGNextAlt
    {189, 149, 46, 22}, // IGLevel
    {19, 64, 59, 8},    // IGStatisticsTitle
    {24, 85, 48, 107},  // IGStatistics
    {48, 88, 23, 103},  // IGStatisticsValues
    {93, 13, 70, 14},   // MOTitle
    {43, 35, 170, 162}, // MOContent
    {95, 40, 81, 162},  // STANDARDGameBoard
    {189, 181, 46, 14} // BurnTimer
};

constexpr int TETROMINO_COUNT = 7;
int tetrominoes[TETROMINO_COUNT][4][4][4] = {
    // I
    {
        {
            {0,0,0,0},
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0}
        },
        {
            {0,0,0,0},
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0}
        }
    },
    // O
    {
        {
            {0,0,0,0},
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0}
        }
    },
    // J
    {
        {
            {0,0,0,0},
            {0,1,1,1},
            {0,0,0,1},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,0,1,1},
            {0,0,1,0},
            {0,0,1,0},
            {0,0,0,0}
        }
    },
    // L
    {
        {
            {0,0,0,0},
            {0,1,1,1},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,0,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,1},
            {0,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,1},
            {0,0,0,0}
        }
    },
    // S
    {
        {
            {0,0,0,0},
            {0,0,1,1},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,1},
            {0,0,0,1},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {0,0,1,1},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,1},
            {0,0,0,1},
            {0,0,0,0}
        }
    },
    // T
    {
        {
            {0,0,0,0},
            {0,1,1,1},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,1},
            {0,0,1,0},
            {0,0,0,0}
        }
    },
    // Z
    {
        {
            {0,0,0,0},
            {0,1,1,0},
            {0,0,1,1},
            {0,0,0,0}
        },
        {
            {0,0,0,1},
            {0,0,1,1},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {0,1,1,0},
            {0,0,1,1},
            {0,0,0,0}
        },
        {
            {0,0,0,1},
            {0,0,1,1},
            {0,0,1,0},
            {0,0,0,0}
        }
    }
    
};
constexpr int colors[7][3] = {
    {0, 255, 255},   // I - Cyan
    {255, 255, 0},   // O - Yellow
    {0, 0, 255},     // J - Blue
    {255, 165, 0},    // L - Orange
    {0, 255, 0},     // S - Green
    {160, 0, 240},   // T - Purple
    {255, 0, 0}     // Z - Red
};
constexpr int statYOffsets[TETROMINO_COUNT] = {
    0,
    15,
    32,
    48,
    64,
    79,
    99
};
struct Piece {
    int type; // 0-6
    int rotation;
    int x;
    int y;
};

constexpr int gravityFrames[] = {
    48, 43, 38, 33, 28, 23, 18, 13, 8, 6,
    5, 5, 5, 4, 4, 4, 3, 3, 3, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2
};
constexpr int garbageLevels[] = {
    50, 50, 50, 40, 40, 40, 30, 30, 30, 30,
    25, 25, 25, 25, 20, 20, 20, 20, 15, 15,
    15, 15, 10, 10, 10, 10, 10, 10, 10
};

constexpr float DAS_INITIAL = 16.0f / 60.0f; // ~0.2667s
constexpr float DAS_REPEAT = 6.0f / 60.0f; // ~0.1s