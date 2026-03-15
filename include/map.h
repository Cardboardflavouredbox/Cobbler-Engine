#pragma once
#include <vector>

#include "types.h"

struct Mapface {
  bool doublesided = false;
  int texture = 0;
  int xloop = 1, yloop = 1;
  std::vector<int> points;
};