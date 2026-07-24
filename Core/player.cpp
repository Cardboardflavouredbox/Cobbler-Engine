#include "player.h"

#include "deltaTime.h"

void inputtoentity(playerinputs input, Entity* entity) {
  float temp = 2.f;
  if (entity->IsGrounded) temp = 1.f;

  entity->dir = input.lookdir;
  entity->movevec2 +=
      (input.movevec2 * entity->movespeed * deltaTime * 10.f / temp);

  if (glm::length(entity->movevec2) > entity->movespeed) {
    entity->movevec2 = glm::normalize(entity->movevec2) * entity->movespeed;
  }

  if (glm::length(entity->movevec2) <
      deltaTime * entity->movespeed * 6.f / temp)
    entity->movevec2 = glm::vec2(0);
  else
    entity->movevec2 -= glm::normalize(entity->movevec2) * deltaTime *
                        entity->movespeed * 6.f / temp;

  // if ((LocalInputs->Shift == 0 && Settings->autorun) ||
  //     (LocalInputs->Shift > 0 && !Settings->autorun))
  //   Camera->movevec3 = (Camera->movevec3 * runspeed);

  if (input.jump == 2 && entity->IsGrounded)
    entity->velocityvec3.z = entity->jumpheight;
}