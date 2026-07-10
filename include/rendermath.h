#pragma once

#include "extern.h"

#ifdef _WIN32
  #ifdef DLLEXPORT
    #define LIB_API __declspec(dllexport)
  #else
    #define LIB_API __declspec(dllimport)
  #endif
#else
#define LIB_API
#endif

extern "C" {
  LIB_API float Areathing(glm::vec2 a, glm::vec2 b, glm::vec2 c);

  LIB_API glm::vec3 GetUV(glm::vec2 P, glm::vec2 R1, glm::vec2 R2, glm::vec2 R3);

  LIB_API float anglething(glm::vec2 a, glm::vec2 b, glm::vec2 c);
  LIB_API bool Vec2inTri(glm::vec2 p, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3);
}