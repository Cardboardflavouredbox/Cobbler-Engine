#pragma once

#include <glm/vec3.hpp>

#include "entity.h"

struct playerinputs {
  glm::vec3 movevec3;
  glm::vec2 lookdir;
  unsigned char jump, attack, altattack;
};

struct playerstate {
  int State = 0;
  float animend = 0, anim = 0;
};

void inputtoentity(playerinputs input, Entity* entity);