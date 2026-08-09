#pragma once

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

struct CameraClass {
  glm::vec3 pos, lookat;
};

LIB_API extern CameraClass* Camera;