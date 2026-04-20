#pragma once

#include "extern.h"

float Areathing(glm::vec2 a, glm::vec2 b, glm::vec2 c);

glm::vec3 GetUV(glm::vec2 P, glm::vec2 R1, glm::vec2 R2, glm::vec2 R3);

float anglething(glm::vec2 a, glm::vec2 b, glm::vec2 c);
bool Vec2inTri(glm::vec2 p, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3);