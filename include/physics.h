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
extern "C" {
LIB_API glm::vec3 movecollisioncheck(
    glm::vec3 hitbox[], glm::vec3 checkposition, float radius, int teamindex,
    float& dist,
    Entity* tempentity);  // returns the face normal

LIB_API void EntityMove(Entity* tempentity);
}