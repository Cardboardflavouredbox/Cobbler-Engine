#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "extern.h"

glm::vec2 SAT3Dto2D(int normal, glm::vec3 point);

float SAT2Dtofloat(int normal, glm::vec2 point);

bool SATcubetri(glm::vec3 aabb[], glm::vec3 tri[]);

bool movecollisioncheck(glm::vec3 hitbox[], glm::vec3 startposition,
                        glm::vec3 endposition);

void EntityMove(std::shared_ptr<Entity> tempentity);