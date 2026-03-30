#pragma once

#include "extern.h"

Vector2 SAT3Dto2D(int normal, Vector3 point);

float SAT2Dtofloat(int normal, Vector2 point);

bool SATcubetri(Vector3 aabb[], Vector3 tri[]);

bool movecollisioncheck(Hitbox hitbox, Vector3 startposition,
                        Vector3 endposition);

void EntityMove(Entity* tempentity);