#pragma once
#include "files.h"
#include "p_displaysettings.h"
#include "p_gameplaysettings.h"
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

struct LIB_API DisplayMenuOption : BasicSelectMenuOption {
  void Interact() { setMenuDisplaySettings(); }
  DisplayMenuOption(int i) {
    name = U"Display Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~DisplayMenuOption() {}
};

struct LIB_API GameplayMenuOption : BasicSelectMenuOption {
  void Interact() { setMenuGameplaySettings(); }
  GameplayMenuOption(int i) {
    name = U"Gameplay Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~GameplayMenuOption() {}
};

struct LIB_API SaveSettingsOption : BasicSelectMenuOption {
  void Interact() { SaveSettings(); }
  SaveSettingsOption(int i) {
    name = U"Save Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~SaveSettingsOption() {}
};

struct LIB_API LoadSettingsOption : BasicSelectMenuOption {
  void Interact() { LoadSettings(); }
  LoadSettingsOption(int i) {
    name = U"Load Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~LoadSettingsOption() {}
};

LIB_API void setMenuSettings() {
  menuindex = 0;
  clearMenuOptionsVector();
  UIGlobalStuff->UIlist.push_back("Menus");
  UIGlobalStuff->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<DisplayMenuOption>(0));
  MenuOptionsVector.push_back(std::make_unique<GameplayMenuOption>(1));
  MenuOptionsVector.push_back(std::make_unique<SaveSettingsOption>(2));
  MenuOptionsVector.push_back(std::make_unique<LoadSettingsOption>(3));
}