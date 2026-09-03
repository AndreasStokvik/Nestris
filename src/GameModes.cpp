#include "GameModes.h"

const char* GameModeNames[] = {
#define X(id, name) name,
    GAMEMODE_LIST
#undef X
};

std::array<bool, (int)GameMode::Count> gameModeValues = {};
GameMode currentMode = GameMode::NES;