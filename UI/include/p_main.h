#pragma once
#include "p_global.h"
#include "p_quit.h"
#include "p_settings.h"
#include "pausemenu.h"
#include "render.h"
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

struct UI_API ResumeMenuOption : BasicSelectMenuOption {
  void Interact() {
    Global->pause = false;
    SDL_SetWindowRelativeMouseMode(RendererGlobal->window, !Global->pause);
  }
  ResumeMenuOption(int i) {
    name = U"Resume";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~ResumeMenuOption() {}
};

struct UI_API SettingsMenuOption : BasicSelectMenuOption {
  void Interact() { setMenuSettings(); }
  SettingsMenuOption(int i) {
    name = U"Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~SettingsMenuOption() {}
};

struct UI_API QuitMenuOption : BasicSelectMenuOption {
  void Interact() { setMenuQuit(); }
  QuitMenuOption(int i) {
    name = U"Quit";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~QuitMenuOption() {}
};

UI_API void setMenuMain() {
  menuindex = 0;
  clearMenuOptionsVector();
  UIGlobalStuff->UIlist.push_back("Menus");
  UIGlobalStuff->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<ResumeMenuOption>(0));
  MenuOptionsVector.push_back(std::make_unique<SettingsMenuOption>(1));
  MenuOptionsVector.push_back(std::make_unique<QuitMenuOption>(2));
}
