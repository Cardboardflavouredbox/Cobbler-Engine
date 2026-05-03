#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct ScreenPoint {
  glm::vec2 p;
  float dist;
  bool isbehindcamera = false;
};

ScreenPoint ToScreenSpace(glm::vec3 P);
glm::vec3 ToWorldSpace(glm::vec2 P);