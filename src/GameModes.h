#pragma once
#include <array>

#define GAMEMODE_LIST \
X(NES, "STANDARD NES RULES") \
X(NESPlus, "NES + ALL QOL OPTIONS") \
X(Garbage, "CLEAR GARBAGE") \
X(BurnDown, "BURN MARATHON")

enum class GameMode {
#define X(id, name) id,
    GAMEMODE_LIST
#undef X
    Count
};

extern const char* GameModeNames[];
extern std::array<bool, (int)GameMode::Count> gameModeValues;
extern GameMode currentMode;