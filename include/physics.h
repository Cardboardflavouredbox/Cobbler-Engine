#pragma once

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "entity.h"

struct LIB_API raycheckresult {
  glm::vec3 A, B;
  float dist = 0;
};

LIB_API raycheckresult capsuleraycheck(glm::vec3 a0, glm::vec3 a1, glm::vec3 b0,
                                       glm::vec3 b1);

struct LIB_API movecollisionresult {
  float dist = 0;
  bool CollidedwithEntityAtAll;
  bool CollidedWithPlayer;
  uint64_t collidedID;
};

extern "C" {
LIB_API glm::vec3 movecollisioncheck(
    glm::vec3 hitbox[], glm::vec3 checkposition, float radius, int teamindex,
    movecollisionresult& resultinfo,
    Entity* movingentity);  // returns the face normal
LIB_API void raycastcheck(glm::vec3 hitbox[], int teamindex,
                          movecollisionresult& resultinfo);
LIB_API void EntityMove(Entity* tempentity);
}