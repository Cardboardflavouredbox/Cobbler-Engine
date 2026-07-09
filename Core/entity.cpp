#include "entity.h"

#include <SDL3/SDL_log.h>

#include <cmath>

#include "extern.h"
#include "model.h"
#include "render.h"

int GetBillBoardIndex(float angle, int lastIndex) {
  // front
  if (angle > -22.5f && angle < 22.6f) return 0;
  if (angle >= 22.5f && angle < 67.5f) return 7;
  if (angle >= 67.5f && angle < 112.5f) return 6;
  if (angle >= 112.5f && angle < 157.5f) return 5;

  // back
  if (angle <= -157.5 || angle >= 157.5f) return 4;
  if (angle >= -157.4f && angle < -112.5f) return 3;
  if (angle >= -112.5f && angle < -67.5f) return 2;
  if (angle >= -67.5f && angle <= -22.5f) return 1;

  return lastIndex;
}
void Entity::rendermodelgroup() {
  renderModelGroup(Modelthing, &ModelGroupMap[Modelthing->name], false);
}