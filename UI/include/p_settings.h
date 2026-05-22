#pragma once
#include "files.h"
#include "p_displaysettings.h"
#include "p_gameplaysettings.h"
#include "p_global.h"
#include "pausemenu.h"
#include "ui.h"

struct DisplayMenuOption : BasicSelectMenuOption {
  void Interact() { setMenuDisplaySettings(); }
  DisplayMenuOption(int i) {
    name = U"Display Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~DisplayMenuOption() {}
};

struct GameplayMenuOption : BasicSelectMenuOption {
  void Interact() { setMenuGameplaySettings(); }
  GameplayMenuOption(int i) {
    name = U"Gameplay Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~GameplayMenuOption() {}
};

struct SaveSettingsOption : BasicSelectMenuOption {
  void Interact() { SaveSettings(); }
  SaveSettingsOption(int i) {
    name = U"Save Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~SaveSettingsOption() {}
};

struct LoadSettingsOption : BasicSelectMenuOption {
  void Interact() { LoadSettings(); }
  LoadSettingsOption(int i) {
    name = U"Load Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~LoadSettingsOption() {}
};

void setMenuSettings() {
  menuindex = 0;
  clearMenuOptionsVector();
  Global->UIlist.push_back("Menus");
  Global->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<DisplayMenuOption>(0));
  MenuOptionsVector.push_back(std::make_unique<GameplayMenuOption>(1));
  MenuOptionsVector.push_back(std::make_unique<SaveSettingsOption>(2));
  MenuOptionsVector.push_back(std::make_unique<LoadSettingsOption>(3));
}