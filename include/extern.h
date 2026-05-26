#pragma once

#include <memory>
#include "entity.h"
#include "global.h"

#ifdef _WIN32
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT
#endif


LIB_EXPORT extern std::unique_ptr<GlobalClass> Global;
LIB_EXPORT extern std::unique_ptr<EditorClass> Editor;
LIB_EXPORT extern std::unique_ptr<SettingsClass> Settings;
LIB_EXPORT extern Inputs* P1Inputs;
LIB_EXPORT extern Entity* Camera;
LIB_EXPORT extern Uint64 lastTime;
LIB_EXPORT extern Uint64 currentTime;
LIB_EXPORT extern std::unordered_map<std::string, Entity* (*)()> SpawnEntities;