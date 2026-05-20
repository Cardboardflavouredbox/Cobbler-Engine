#pragma once
#include "p_global.h"
#include "p_settings.h"
#include "pausemenu.h"
#include "ui.h"

struct ResumeMenuOption : BasicSelectMenuOption {
  void Interact() { Global->pause = false; }
  ResumeMenuOption(int i) {
    name = "Resume";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~ResumeMenuOption() {}
};

struct SettingsMenuOption : BasicSelectMenuOption {
  void Interact() { /*MenuOptionsVector.push_back();*/ }
  SettingsMenuOption(int i) {
    name = "Settings";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~SettingsMenuOption() {}
};
