#pragma once

#include <deque>
#include <glm/vec3.hpp>

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

struct DamageLocation {
  glm::vec3 position;
  float lifestart = 2.f, lifetime = 2.f;
};

LIB_API extern std::deque<DamageLocation> LocalDamageLocations;