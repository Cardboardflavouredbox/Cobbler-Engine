#include "player.h"

#include "deltaTime.h"

// function that turns inputs into entity movement.
void inputtoentity(playerinputs input, Entity* entity) {
  float temp = 4.f;
  if (entity->IsGrounded) temp = 1.f;

  entity->dir = input.lookdir;

  float movelen = glm::length(entity->movevec2);

  if (movelen < 0.5f)
    entity->movevec2 = glm::vec2(0);
  else
    entity->movevec2 *= std::pow(0.0078125f * temp, deltaTime);

  entity->movevec2 +=
      (input.movevec2 * entity->movespeed * deltaTime * 10.f / temp);

  if (glm::length(entity->movevec2) > entity->movespeed) {
    entity->movevec2 = glm::normalize(entity->movevec2) * entity->movespeed;
  }

  if (input.jump == 2 && entity->IsGrounded)
    entity->velocityvec3.z = entity->jumpheight;
}