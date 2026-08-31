#include "player.h"

#include "deltaTime.h"

// function that turns inputs into entity movement.
void inputtoentity(playerinputs input, Entity* entity) {
  entity->dir = input.lookdir;

  float movelen = glm::length(entity->movevec2);

  if (movelen < 0.0001f)
    entity->movevec2 = glm::vec2(0);
  else
    entity->movevec2 *=
        std::pow(entity->IsGrounded ? 0.001f : 0.125f, updatedeltaTime);

  entity->movevec2 += (input.movevec2 * entity->movespeed * updatedeltaTime *
                       10.f / (entity->IsGrounded ? 1.f : 4.f));

  if (glm::length(entity->movevec2) > entity->movespeed) {
    entity->movevec2 = glm::normalize(entity->movevec2) * entity->movespeed;
  }

  if (input.jump == 2 && entity->IsGrounded)
    entity->velocityvec3.z = entity->jumpheight;
}