#include "extern.h"
#include "pausemenu.h"
#include "ui_index.h"

int menuindex = 0;
int currentmenusize = 5;
PauseMenu* currentpausemenu = nullptr;

struct ResumeMenuOption : MenuOptions {
  void Interact() { Global->pause = false; }
  ~ResumeMenuOption() {}
};

struct MainPauseMenu : PauseMenu {
  void ExitMenu() {}
  MainPauseMenu() { MenuOptionsVector.push_back(new ResumeMenuOption()); }
  ~MainPauseMenu() {}
};

void UIupdate() {
  if (P1Inputs->Enter == 2) {
  }
  if (P1Inputs->W == 2) {
    menuindex--;
    if (menuindex < 0) menuindex = currentmenusize - 1;
  }
  if (P1Inputs->S == 2) {
    menuindex++;
    if (menuindex >= currentmenusize) menuindex = 0;
  }
}

void changeUIindex() {
  UIupdate();
  if (Global->pause) {
    if (currentpausemenu == nullptr) {
      currentpausemenu = new MainPauseMenu();
    }
    Global->UIlist[0] = "Pause";
  } else
    Global->UIlist[0] = "Pistol";
}