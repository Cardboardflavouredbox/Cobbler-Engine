#pragma once
#include "pausemenu.h"
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

struct UI_API BasicSelectMenuOption : MenuOptions {
  BasicSelectMenuOption() {
    MenuOptionTextChanger* temp = new MenuOptionTextChanger();
    temp->optionname = &name;
    temp->index = &index;
    UItext* text = new UItext(temp);
    text->anchor = UIthing::lefttop;
    text->color = 11;
    text->rgba = glm::vec4(1, 1, 1, 1);
    text->pos = glm::vec2(16, 12 * (index + 1));
    UIGlobalStuff->UImap["Menus"].push_back(text);
    UIcomponent = text;
  }
};

struct UI_API CheckBoxMenuOption : MenuOptions {
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
    UIGlobalStuff->UImap["Menus"].push_back(text);
    UIcomponent = text;
  }
};

UI_API void setMenuMain();
UI_API void setMenuQuit();
UI_API void setMenuSettings();
UI_API void setMenuGameplaySettings();
UI_API void setMenuDisplaySettings();