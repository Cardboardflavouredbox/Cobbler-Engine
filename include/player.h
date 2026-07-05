#pragma once

#include <array>
#include <glm/vec3.hpp>

#include "entity.h"

struct playerinputs {
  glm::vec3 movevec3;
  glm::vec2 lookdir;
  unsigned char jump, attack, altattack;
};

struct playerdatapacket {
  std::array<float, 3> movevec3, position, velocityvec3;
  std::array<float, 2> lookdir;
  unsigned char jump, attack, altattack;
  bool IsGrounded;
  void Set(playerinputs* input) {
    for (int i = 0; i < 3; i++) {
      movevec3[i] = input->movevec3[i];
    }
    for (int i = 0; i < 2; i++) {
      lookdir[i] = input->lookdir[i];
    }
    jump = input->jump;
    attack = input->attack;
    altattack = input->altattack;
  }
};

playerinputs Loadinputdata(playerdatapacket input);

struct playerstate {
  int State = 0;
  float animend = 0, anim = 0;
};

void inputtoentity(playerinputs input, Entity* entity);