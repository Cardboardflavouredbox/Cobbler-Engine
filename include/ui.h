#pragma once

#include <string>

#include "types.h"

struct UIthing {
  Vector2 pos;
  virtual void update() = 0;
  virtual void render() = 0;
};

struct UIbox : public UIthing {
  unsigned char color;
  Vector2 size;
  void update();
  void render();
};

struct UITextChanger {
  std::string* string;
  virtual void update() = 0;
};

struct UItext : public UIthing {
  unsigned char color;
  UITextChanger* TextChanger = nullptr;
  std::string string;
  void update();
  void render();
};