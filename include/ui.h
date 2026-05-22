#pragma once

#include <glm/vec2.hpp>

struct UIthing {
  enum anchorpos {
    lefttop,
    middletop,
    righttop,
    leftmiddle,
    middlemiddle,
    rightmiddle,
    leftbottom,
    middlebottom,
    rightbottom
  };
  glm::vec2 pos;
  anchorpos anchor = middlemiddle;
  virtual void update() = 0;
  virtual void render() = 0;
  virtual ~UIthing() {}  // delete thingy
};