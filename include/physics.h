#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "extern.h"

bool movecollisioncheck(glm::vec3 hitbox[], glm::vec3 checkposition,
                        float radius);

void EntityMove(Entity* tempentity);