#include "extern.h"

#include <SDL3/SDL_timer.h>

#include "deltaTime.h"
#include "font.h"
#include "model.h"

float deltaTime;

std::unique_ptr<GlobalClass> Global;
std::unique_ptr<EditorClass> Editor;
std::unique_ptr<SettingsClass> Settings;
Inputs* LocalInputs;
playerinputs* P1PlayerInputs;
CameraEntity* Camera;
Uint64 lastTime;
Uint64 currentTime = SDL_GetPerformanceCounter();
std::unordered_map<std::string, Entity* (*)()> SpawnEntities;
std::unordered_map<std::string, void (*)()> PlayerClassUpdate;
FreetypeClass* Freetypething;
std::unordered_map<std::string, ModelGroupClass> ModelGroupMap;