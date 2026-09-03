#pragma once
#include <array>

// --- MODIFIERS ---
#define MOD_LIST \
X(ShowGrid, "SHOW GRID") \
X(HardDrop, "HARD DROP") \
X(ShowGhost, "SHOW GHOST") \
X(SevenBag, "ENABLE 7-BAG") \
X(EnableHold, "ENABLE HOLD") \
X(RandomRotation, "RANDOM ROTATION")

enum class ModID {
#define X(id, name) id,
    MOD_LIST
#undef X
    Count
};

extern const char* ModNames[];
extern std::array<bool, (int)ModID::Count> modValues;

bool isModOn(ModID id);
void toggleMod(ModID id);