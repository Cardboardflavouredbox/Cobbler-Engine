#include "player.h"

void inputtoentity(playerinputs input, Entity* entity) {
  entity->dir = input.lookdir;
  entity->movevec3 = (input.movevec3 * entity->movespeed);

  // if ((LocalInputs->Shift == 0 && Settings->autorun) ||
  //     (LocalInputs->Shift > 0 && !Settings->autorun))
  //   Camera->movevec3 = (Camera->movevec3 * runspeed);

  if (input.jump == 2 && entity->IsGrounded)
    entity->velocityvec3.z = entity->jumpheight;
}