#pragma once
#include "p_global.h"
#include "pausemenu.h"
#include "ui.h"

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

struct LIB_API QuitYes : BasicSelectMenuOption {
  void Interact() { IsRunning = false; }
  QuitYes(int i) {
    name = U"Yes";
    index = i;
    UIcomponent->pos = glm::vec2(0, 12 * (index));
    UIcomponent->anchor = UIthing::middlemiddle;
  }
  ~QuitYes() {}
};

struct LIB_API QuitNo : BasicSelectMenuOption {
  void Interact() { setMenuMain(); }
  QuitNo(int i) {
    name = U"No";
    index = i;
    UIcomponent->pos = glm::vec2(0, 12 * (index));
    UIcomponent->anchor = UIthing::middlemiddle;
  }
  ~QuitNo() {}
};

LIB_API void setMenuQuit() {
  menuindex = 1;
  clearMenuOptionsVector();
  UIGlobalStuff->UIlist.push_back("Menus");
  UIGlobalStuff->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<QuitYes>(0));
  MenuOptionsVector.push_back(std::make_unique<QuitNo>(1));
}