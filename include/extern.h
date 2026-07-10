#pragma once

#include <memory>

#include "global.h"
#include "player.h"

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

LIB_API extern std::unique_ptr<GlobalClass> Global;
LIB_API extern std::unique_ptr<EditorClass> Editor;
LIB_API extern std::unique_ptr<SettingsClass> Settings;
LIB_API extern Inputs* LocalInputs;
LIB_API extern playerinputs* P1PlayerInputs;
LIB_API extern CameraEntity* Camera;
LIB_API extern Uint64 lastTime;
LIB_API extern Uint64 currentTime;
LIB_API extern std::unordered_map<std::string, Entity* (*)()> SpawnEntities;
LIB_API extern std::unordered_map<std::string, void (*)()> PlayerClassUpdate;