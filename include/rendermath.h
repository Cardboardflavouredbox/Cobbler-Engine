#pragma once

#include "extern.h"

#ifdef _WIN32
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT
#endif

LIB_EXPORT float Areathing(glm::vec2 a, glm::vec2 b, glm::vec2 c);

LIB_EXPORT glm::vec3 GetUV(glm::vec2 P, glm::vec2 R1, glm::vec2 R2, glm::vec2 R3);

LIB_EXPORT float anglething(glm::vec2 a, glm::vec2 b, glm::vec2 c);
LIB_EXPORT bool Vec2inTri(glm::vec2 p, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3);