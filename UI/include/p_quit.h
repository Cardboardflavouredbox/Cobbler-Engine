#pragma once
#include "p_global.h"
#include "pausemenu.h"
#include "ui.h"

struct QuitYes : BasicSelectMenuOption {
  void Interact() { Global->IsRunning = false; }
  QuitYes(int i) {
    name = "Yes";
    index = i;
    UIcomponent->pos = glm::vec2(0, 12 * (index));
    UIcomponent->anchor = UIthing::middlemiddle;
  }
  ~QuitYes() {}
};

struct QuitNo : BasicSelectMenuOption {
  void Interact() { setMenuMain(); }
  QuitNo(int i) {
    name = "No";
    index = i;
    UIcomponent->pos = glm::vec2(0, 12 * (index));
    UIcomponent->anchor = UIthing::middlemiddle;
  }
  ~QuitNo() {}
};

void setMenuQuit() {
  menuindex = 1;
  clearMenuOptionsVector();
  Global->UIlist.push_back("Menus");
  Global->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<QuitYes>(0));
  MenuOptionsVector.push_back(std::make_unique<QuitNo>(1));
}