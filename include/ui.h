#pragma once

#include <glm/vec2.hpp>
#include <memory>

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

struct UIGlobalClass {
  std::unordered_map<std::string, std::vector<UIthing*>> UImap;
  std::vector<std::string> UIlist = {"default"};

  std::unordered_map<std::string, std::vector<UI3Dthing*>> UImap3D;
};

LIB_API extern std::unique_ptr<UIGlobalClass> UIGlobalStuff;

extern "C" {
LIB_API bool UIsetup();
LIB_API void UIfree();
LIB_API void changeUIindex();
}