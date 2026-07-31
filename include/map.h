#pragma once
#include <array>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

struct MapPoint {
  glm::vec3 pos;
  std::array<unsigned char, 3> shade = {255, 255, 255};
};

struct Mapface {
  bool doublesided = false;
  std::string texture;
  int xloop = 1, yloop = 1;
  std::vector<int> points;
  std::vector<glm::vec2> UVs;
};