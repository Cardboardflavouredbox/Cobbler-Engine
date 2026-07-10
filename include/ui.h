#pragma once

#include <glm/vec2.hpp>

#include "model.h"

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

struct UI3Dthing {
  Modeltransform* modeltrans;
  virtual void update() = 0;
  virtual ~UI3Dthing() { delete modeltrans; }
};