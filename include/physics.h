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
LIB_API float Entityraycheck(
    glm::vec3 ray[], Entity* checkentity);  // returns dist (-1 if not collided)
LIB_API void EntityMove(Entity* tempentity);
LIB_API glm::vec3 closestPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b,
                                       glm::vec3 c);
LIB_API bool BSPthing(glm::vec3 pos, float radius, int index,
                      std::vector<std::array<uint32_t, 3>>& out);
LIB_API void recursiveBSPadd(int index,
                             std::vector<std::array<uint32_t, 3>>& out);
}