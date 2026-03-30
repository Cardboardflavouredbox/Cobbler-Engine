#pragma once

#include <string>

#include "types.h"

struct UIthing {
  Vector2 pos;
  virtual void update() = 0;
  virtual void render(unsigned char* pixels, unsigned char pixelsdepth[],
                      int pitch) = 0;
};

struct UIbox : public UIthing {
  unsigned char color;
  Vector2 size;
  void update();
  void render(unsigned char* pixels, unsigned char pixelsdepth[], int pitch);
};

struct UItext : public UIthing {
  unsigned char color;
  std::string string;
  void update();
  void render(unsigned char* pixels, unsigned char pixelsdepth[], int pitch);
};