#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#ifdef _WIN32
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT
#endif

struct ScreenPoint {
  glm::vec2 p;
  float dist;
  bool isbehindcamera = false;
};

LIB_EXPORT ScreenPoint ToScreenSpace(glm::vec3 P);
LIB_EXPORT glm::vec3 ToWorldSpace(glm::vec2 P);