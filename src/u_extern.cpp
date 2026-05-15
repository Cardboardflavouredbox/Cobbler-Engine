#include <SDL3/SDL_timer.h>

#include "extern.h"

GlobalClass* Global;
EditorClass* Editor;
SettingsClass* Settings;
Inputs* P1Inputs;
Entity* Camera;
ZipData* LoadedData;
Uint64 lastTime;
Uint64 currentTime = SDL_GetPerformanceCounter();