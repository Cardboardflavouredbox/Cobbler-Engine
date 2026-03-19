#pragma once
#include "types.h"

struct Entity {
 public:
  float hp;
  Vector2 dir;
  Vector3 position, moveVector3 = Vector3({0, 0, 0}),
                    velocityVector3 = Vector3({0, 0, 0});
  float gravity = 9.81f;
  Capsule hitbox;
};