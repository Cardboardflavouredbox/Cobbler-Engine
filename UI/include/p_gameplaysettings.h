#pragma once
#include "files.h"
#include "p_global.h"
#include "pausemenu.h"
#include "ui.h"

struct AutorunOption : CheckBoxMenuOption {
  void Interact() { Settings->autorun = !Settings->autorun; }
  AutorunOption(int i) : CheckBoxMenuOption(&Settings->autorun) {
    name = U"Autorun";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~AutorunOption() {}
};

void setMenuGameplaySettings() {
  menuindex = 0;
  clearMenuOptionsVector();
  Global->UIlist.push_back("Menus");
  Global->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<AutorunOption>(0));
}