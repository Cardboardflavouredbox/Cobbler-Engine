#pragma once
#include "files.h"
#include "p_global.h"
#include "pausemenu.h"
#include "ui.h"

struct DisplayMenuOption : BasicSelectMenuOption {
  void Interact() {}
  DisplayMenuOption(int i) {
    name = "Display Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~DisplayMenuOption() {}
};

struct GameplayMenuOption : BasicSelectMenuOption {
  void Interact() {}
  GameplayMenuOption(int i) {
    name = "Gameplay Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~GameplayMenuOption() {}
};

struct SaveSettingsOption : BasicSelectMenuOption {
  void Interact() { SaveSettings(); }
  SaveSettingsOption(int i) {
    name = "Save Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~SaveSettingsOption() {}
};

void setMenuSettings() {
  menuindex = 0;
  clearMenuOptionsVector();
  Global->UIlist.push_back("Menus");
  Global->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<DisplayMenuOption>(0));
  MenuOptionsVector.push_back(std::make_unique<GameplayMenuOption>(1));
  MenuOptionsVector.push_back(std::make_unique<SaveSettingsOption>(2));
}