#pragma once
#include "p_global.h"
#include "pausemenu.h"
#include "ui.h"

#ifdef _WIN32
#ifdef UIEXPORT
#define UI_API __declspec(dllexport)
#else
#define UI_API __declspec(dllimport)
#endif
#else
#define UI_API
#endif

struct UI_API QuitYes : BasicSelectMenuOption {
  void Interact() { IsRunning = false; }
  QuitYes(int i) {
    name = U"Yes";
    index = i;
    UIcomponent->pos = glm::vec2(0, 12 * (index));
    UIcomponent->anchor = UIthing::middlemiddle;
  }
  ~QuitYes() {}
};

struct UI_API QuitNo : BasicSelectMenuOption {
  void Interact() { setMenuMain(); }
  QuitNo(int i) {
    name = U"No";
    index = i;
    UIcomponent->pos = glm::vec2(0, 12 * (index));
    UIcomponent->anchor = UIthing::middlemiddle;
  }
  ~QuitNo() {}
};

UI_API void setMenuQuit() {
  menuindex = 1;
  clearMenuOptionsVector();
  UIGlobalStuff->UIlist.push_back("Menus");
  UIGlobalStuff->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<QuitYes>(0));
  MenuOptionsVector.push_back(std::make_unique<QuitNo>(1));
}