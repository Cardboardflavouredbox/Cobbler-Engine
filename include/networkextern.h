#pragma once
#include <map>
#include <set>

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

  struct PlayerNetClass {
    float Timecounter, deltatimelocal;
    Entity* PlayerEntity;
    playerinputs PlayerInput;
  };
  std::map<uint64_t, PlayerNetClass> PlayerNetStuff;

  float Onlinesendwait = 0.05f;
};
LIB_API extern std::unique_ptr<GlobalNetworkClass> GlobalNetworkStuff;
