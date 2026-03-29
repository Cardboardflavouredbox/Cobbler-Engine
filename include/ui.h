#pragma once

#include "types.h"

struct UIthing {
  Vector2 pos;
  virtual void update() {}
  virtual void render(unsigned char* pixels, unsigned char pixelsdepth[],
                      int pitch) {}
};

struct UIbox : public UIthing {
  unsigned char color;
  Vector2 size;
  void update() override;
  void render(unsigned char* pixels, unsigned char pixelsdepth[],
              int pitch) override;
};