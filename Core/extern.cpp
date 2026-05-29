#include "extern.h"

#include <SDL3/SDL_timer.h>

#include "font.h"

std::unique_ptr<GlobalClass> Global;
std::unique_ptr<EditorClass> Editor;
std::unique_ptr<SettingsClass> Settings;
Inputs* P1Inputs;
CameraEntity* Camera;
Uint64 lastTime;
Uint64 currentTime = SDL_GetPerformanceCounter();
std::unordered_map<std::string, Entity* (*)()> SpawnEntities;
FreetypeClass* Freetypething;