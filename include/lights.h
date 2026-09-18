#pragma once

#include <glm/vec3.hpp>
#include <map>
#include <queue>

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

LIB_API extern std::map<uint32_t, glm::vec3> Lights;
LIB_API extern std::queue<uint32_t> LightdeleteQueue;

extern "C" {
LIB_API uint32_t LightMapEmptyIndex();
}