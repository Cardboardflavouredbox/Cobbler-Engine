#pragma once

#include <glm/vec2.hpp>

struct UIthing {
  glm::vec2 pos;
  virtual void update() = 0;
  virtual void render() = 0;
  virtual ~UIthing() {}  // delete thingy
};

bool UIsetup();