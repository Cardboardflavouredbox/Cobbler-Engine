#pragma once

#include <array>
#include <cstdint>
#include <glm/vec3.hpp>

#include "entity.h"

struct playerinputs {
  glm::vec2 lookdir, movevec2;
  unsigned char jump, attack, altattack;
};

struct playerdatapacket {
  int State;
  // float Stateanimend = 0, Stateanim = 0;

  uint64_t ID;
  std::array<float, 3> position, velocityvec3;
  std::array<float, 2> movevec2, lookdir;
  unsigned char jump, attack, altattack;
  bool IsGrounded;
  void Set(playerinputs* input) {
    for (int i = 0; i < 2; i++) {
      movevec2[i] = input->movevec2[i];
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