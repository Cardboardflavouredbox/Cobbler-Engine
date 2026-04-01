#pragma once
#include <array>
#include <vector>

#include "types.h"

struct Mapface {
  bool doublesided = false;
  int texture = 0;
  int xloop = 1, yloop = 1;
  std::vector<int> points;
  std::vector<Vector2> UVs;
  std::array<unsigned char, 4> shade = {255, 255, 255, 255};
};