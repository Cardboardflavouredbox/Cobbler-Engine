#pragma once

#include <glm/vec3.hpp>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

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
  sol::state lua;
  sol::protected_function luaupdate;
  glm::vec3 pos, lookat, right, up;
};

LIB_API extern CameraClass* Camera;

LIB_API glm::vec3 LookdirToVec3(const glm::vec2& lookdir);