#include "extern.h"
#include "g_ui.h"
#include "pausemenu.h"
#include "ui_index.h"

int menuindex = 0;
PauseMenu* currentpausemenu = nullptr;

struct ResumeMenuOption : MenuOptions {
  void Interact() { Global->pause = false; }
  ResumeMenuOption() {
    name = "Resume";
    MenuOptionTextChanger* temp = new MenuOptionTextChanger(name);
    temp->index = 0;
    UItext* text = new UItext(temp);
    text->color = 11;
    text->rgba = glm::vec4(1, 1, 1, 1);
    text->pos = glm::vec2(16, 12);
    Global->UImap["Pause"].push_back(text);
  }
  ~ResumeMenuOption() {}
};

struct MainPauseMenu : PauseMenu {
  void ExitMenu() {}
  MainPauseMenu() { MenuOptionsVector.push_back(new ResumeMenuOption()); }
  ~MainPauseMenu() {}
};

void UIupdate() {
  if (P1Inputs->Enter == 2) {
    currentpausemenu->MenuOptionsVector[menuindex]->Interact();
  }
  if (P1Inputs->W == 2) {
    menuindex--;
    if (menuindex < 0)
      menuindex = currentpausemenu->MenuOptionsVector.size() - 1;
  }
  if (P1Inputs->S == 2) {
    menuindex++;
    if (menuindex >= currentpausemenu->MenuOptionsVector.size()) menuindex = 0;
  }
}

void changeUIindex() {
  if (Global->pause) {
    if (currentpausemenu == nullptr) {
      currentpausemenu = new MainPauseMenu();
    }
    UIupdate();
    Global->UIlist[0] = "Pause";
  } else
    Global->UIlist[0] = "Pistol";
}