#pragma once

#include <glm/vec2.hpp>
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

struct ScreenPoint {
  glm::vec2 p;
  float dist;
  bool isbehindcamera = false;
};

extern "C" {
  LIB_API ScreenPoint ToScreenSpace(glm::vec3 P);
  LIB_API glm::vec3 ToWorldSpace(glm::vec2 P);
}