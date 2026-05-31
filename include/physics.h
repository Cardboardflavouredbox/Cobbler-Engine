#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "extern.h"

glm::vec3 movecollisioncheck(glm::vec3 hitbox[], glm::vec3 checkposition,
                             float radius);  // returns the face normal
bool capsuleraycheck(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4,
                     float radius);

void EntityMove(Entity* tempentity);