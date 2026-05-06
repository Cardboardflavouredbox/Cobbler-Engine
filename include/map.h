#pragma once
#include <array>
#include <glm/vec2.hpp>
#include <vector>

struct Mapface {
  bool doublesided = false;
  int texture = 0;
  int xloop = 1, yloop = 1;
  std::vector<int> points;
  std::vector<glm::vec2> UVs;
  std::array<unsigned char, 4> shade = {255, 255, 255, 255};
};