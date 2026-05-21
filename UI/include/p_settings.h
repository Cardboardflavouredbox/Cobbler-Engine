#pragma once
#include "p_global.h"
#include "pausemenu.h"
#include "ui.h"

struct GraphicsMenuOption : BasicSelectMenuOption {
  void Interact() {}
  GraphicsMenuOption(int i) {
    name = "Resume";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~GraphicsMenuOption() {}
};

struct SaveSettings : BasicSelectMenuOption {
  void Interact() {}
  SaveSettings(int i) {
    name = "Resume";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~SaveSettings() {}
};

void setMenuSettings() {
  menuindex = 0;
  clearMenuOptionsVector();
  Global->UIlist.push_back("Menus");
  Global->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<GraphicsMenuOption>(0));
  MenuOptionsVector.push_back(std::make_unique<SaveSettings>(1));
}