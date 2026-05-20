#pragma once
#include "pausemenu.h"
#include "ui.h"

struct BasicSelectMenuOption : MenuOptions {
  BasicSelectMenuOption() {
    MenuOptionTextChanger* temp = new MenuOptionTextChanger();
    temp->optionname = &name;
    temp->index = &index;
    std::shared_ptr<UItext> text(std::make_shared<UItext>(temp));
    text->color = 11;
    text->rgba = glm::vec4(1, 1, 1, 1);
    text->pos = glm::vec2(16, 12 * (index + 1));
    Global->UImap["Menus"].push_back(text);
    UIcomponent = text;
  }
};