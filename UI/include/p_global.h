#pragma once
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

struct LIB_API BasicSelectMenuOption : MenuOptions {
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

struct LIB_API CheckBoxMenuOption : MenuOptions {
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

LIB_API void setMenuMain();
LIB_API void setMenuQuit();
LIB_API void setMenuSettings();
LIB_API void setMenuGameplaySettings();
LIB_API void setMenuDisplaySettings();