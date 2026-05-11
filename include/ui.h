#pragma once

#include <glm/vec2.hpp>
#include <string>

struct UIthing {
  glm::vec2 pos;
  virtual void update() = 0;
  virtual void render() = 0;
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

struct UIImageUVIndexChanger {
  int* index;
  virtual void update() = 0;
};

struct ImagePistolChanger : UIImageUVIndexChanger {
  float animlen[3] = {0.f, 1.f, 2.f}, animprogress = 0.f;
  unsigned char anim = 0;
  void update();
};

struct UIbox : public UIthing {
  unsigned char color;
  glm::vec2 size;
  void update();
  void render();
};

struct UIimage : public UIthing {
  unsigned char color;
  UIImageUVIndexChanger* UVIndexChanger = nullptr;
  glm::vec2 size;
  std::pair<glm::vec2, glm::vec2>* uvlist;
  int textureindex, uvindex = 0;
  void update();
  void render();
};

struct UItext : public UIthing {
  unsigned char color;
  UITextChanger* TextChanger = nullptr;
  std::string string;
  void TextChangerSet(UITextChanger* newTextChanger);
  void update();
  void render();
};