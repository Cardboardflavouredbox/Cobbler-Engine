#pragma once

#include <glm/vec3.hpp>

#include "entity.h"

struct playerinputs {
  glm::vec3 movevec3;
  glm::vec2 lookdir;
  unsigned char jump, attack, altattack;
};

void inputtoentity(playerinputs input, Entity* entity);