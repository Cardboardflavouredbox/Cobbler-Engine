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
LIB_API extern playerinputs* P1PlayerInputs;
LIB_API extern Entity* LocalPlayer;
LIB_API extern uint64_t lastTime;
LIB_API extern uint64_t currentTime;
LIB_API extern std::unordered_map<std::string, Entity* (*)(unsigned int)>
    SpawnEntities;
LIB_API extern std::unordered_map<std::string, void (*)()> PlayerClassUpdate;
LIB_API extern std::unique_ptr<GlobalNetworkClass> GlobalNetworkStuff;

extern "C" {
LIB_API void EntitySpawn();
LIB_API unsigned int EntityMapEmptyIndex();
LIB_API unsigned int ParticleMapEmptyIndex();
}