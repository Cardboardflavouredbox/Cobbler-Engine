#pragma once
#include "files.h"
#include "p_global.h"
#include "pausemenu.h"
#include "ui.h"

struct VsyncOption : CheckBoxMenuOption {
  void Interact() { Settings->vsync = !Settings->vsync; }
  VsyncOption(int i) : CheckBoxMenuOption(&Settings->vsync) {
    name = U"Vsync";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~VsyncOption() {}
};

void setMenuDisplaySettings() {
  menuindex = 0;
  clearMenuOptionsVector();
  Global->UIlist.push_back("Menus");
  Global->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<VsyncOption>(0));
}