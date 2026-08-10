#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "extern.h"

glm::vec3 movecollisioncheck(glm::vec3 hitbox[], glm::vec3 checkposition,
                             float radius,
                             int teamindex);  // returns the face normal

struct raycheckresult {
  glm::vec3 A, B;
  float dist = 0;
};

raycheckresult capsuleraycheck(glm::vec3 a0, glm::vec3 a1, glm::vec3 b0,
                               glm::vec3 b1);

void EntityMove(Entity* tempentity);