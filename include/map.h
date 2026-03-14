#pragma once
#include <vector>

#include "types.h"

struct Mapface {
  bool doublesided = false;
  int texture = 0;
  std::vector<int> points;
};