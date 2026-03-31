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

struct TextandNumChanger : UITextChanger {
  std::string text;
  int* num;
  void update();
};

struct TextandGlobalFacePointChanger : UITextChanger {
  std::string text;
  int* num;
  int pointindex;
  void update();
};

struct TextandGlobalPointChanger : UITextChanger {
  std::string text;
  int* num;
  int xyz;  // 0==x,1==y,2==z
  void update();
};

struct UItext : public UIthing {
  unsigned char color;
  UITextChanger* TextChanger = nullptr;
  std::string string;
  void TextChangerSet(UITextChanger* newTextChanger);
  void update();
  void render();
};