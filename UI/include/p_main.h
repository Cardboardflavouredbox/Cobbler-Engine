#pragma once
#include "p_global.h"
#include "p_quit.h"
#include "p_settings.h"
#include "pausemenu.h"
#include "ui.h"

struct ResumeMenuOption : BasicSelectMenuOption {
  void Interact() {
    Global->pause = false;
    SDL_SetWindowRelativeMouseMode(Global->window, !Global->pause);
  }
  ResumeMenuOption(int i) {
    name = U"Resume";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~ResumeMenuOption() {}
};

struct SettingsMenuOption : BasicSelectMenuOption {
  void Interact() { setMenuSettings(); }
  SettingsMenuOption(int i) {
    name = U"Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~SettingsMenuOption() {}
};

struct QuitMenuOption : BasicSelectMenuOption {
  void Interact() { setMenuQuit(); }
  QuitMenuOption(int i) {
    name = U"Quit";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~QuitMenuOption() {}
};

void setMenuMain() {
  menuindex = 0;
  clearMenuOptionsVector();
  Global->UIlist.push_back("Menus");
  Global->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<ResumeMenuOption>(0));
  MenuOptionsVector.push_back(std::make_unique<SettingsMenuOption>(1));
  MenuOptionsVector.push_back(std::make_unique<QuitMenuOption>(2));
}
