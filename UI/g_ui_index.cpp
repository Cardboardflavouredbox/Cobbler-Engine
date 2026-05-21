#include "extern.h"
#include "g_ui.h"
#include "p_main.h"
#include "pausemenu.h"
#include "ui_index.h"

int menuindex = 0;
std::vector<std::unique_ptr<MenuOptions>> MenuOptionsVector;

void UIupdate() {
  if (P1Inputs->Enter == 2) {
    MenuOptionsVector[menuindex]->Interact();
  }
  if (P1Inputs->W == 2) {
    menuindex--;
    if (menuindex < 0) menuindex = MenuOptionsVector.size() - 1;
  }
  if (P1Inputs->S == 2) {
    menuindex++;
    if (menuindex >= MenuOptionsVector.size()) menuindex = 0;
  }
}

void changeUIindex() {
  if (Global->pause) {
    if (MenuOptionsVector.empty()) {
      Global->UIlist.push_back("Menus");
      Global->UImap["Menus"];

      MenuOptionsVector.push_back(std::make_unique<ResumeMenuOption>(0));
      MenuOptionsVector.push_back(std::make_unique<SettingsMenuOption>(1));
    }
    UIupdate();
    Global->UIlist[0] = "Pause";
  } else {
    if (!MenuOptionsVector.empty()) {
      clearMenuOptionsVector();
    }
    Global->UIlist[0] = "Pistol";
  }
}