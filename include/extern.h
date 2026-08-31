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
LIB_API extern std::unordered_map<std::string, void (*)()> PlayerClassUpdate;