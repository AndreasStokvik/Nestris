#include "Mods.h"

const char* ModNames[] = {
#define X(id, name) name,
    MOD_LIST
#undef X
};

std::array<bool, (int)ModID::Count> modValues = {};

bool isModOn(ModID id) { return modValues[(int)id]; }
void toggleMod(ModID id) { modValues[(int)id] = !modValues[(int)id]; }