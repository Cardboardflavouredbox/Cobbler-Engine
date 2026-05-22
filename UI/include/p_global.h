#pragma once
#include "pausemenu.h"
#include "ui.h"

struct BasicSelectMenuOption : MenuOptions {
  BasicSelectMenuOption() {
    MenuOptionTextChanger* temp = new MenuOptionTextChanger();
    temp->optionname = &name;
    temp->index = &index;
    UItext* text = new UItext(temp);
    text->anchor = UIthing::lefttop;
    text->color = 11;
    text->rgba = glm::vec4(1, 1, 1, 1);
    text->pos = glm::vec2(16, 12 * (index + 1));
    Global->UImap["Menus"].push_back(text);
    UIcomponent = text;
  }
};

struct CheckBoxMenuOption : MenuOptions {
  CheckBoxMenuOption(bool* enabledcheck) {
    MenuCheckboxTextChanger* temp = new MenuCheckboxTextChanger();
    temp->optionname = &name;
    temp->index = &index;
    temp->enabled = enabledcheck;
    UItext* text = new UItext(temp);
    text->anchor = UIthing::lefttop;
    text->color = 11;
    text->rgba = glm::vec4(1, 1, 1, 1);
    text->pos = glm::vec2(16, 12 * (index + 1));
    Global->UImap["Menus"].push_back(text);
    UIcomponent = text;
  }
};

void setMenuMain();
void setMenuQuit();
void setMenuSettings();
void setMenuGameplaySettings();