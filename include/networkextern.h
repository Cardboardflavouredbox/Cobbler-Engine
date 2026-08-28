#pragma once
#include <set>
#include <unordered_map>

#include "entity.h"
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

struct GlobalNetworkClass {
  std::set<uint64_t> UserIDs;
  std::unordered_map<uint64_t, float> PlayerTimecounter;
  std::unordered_map<uint64_t, Entity*> PlayerEntity;
  std::unordered_map<uint64_t, playerinputs> PlayerInputList;

  float Onlinesendwait = 0.05f;
};
LIB_API extern std::unique_ptr<GlobalNetworkClass> GlobalNetworkStuff;
