#pragma once
#include <vector>

struct MenuOptions {
  virtual void Interact() = 0;
  virtual ~MenuOptions() {}
};

struct PauseMenu {
  std::vector<MenuOptions*> MenuOptionsVector;
  virtual void ExitMenu() = 0;
  virtual ~PauseMenu() {
    for (int i = 0; i < MenuOptionsVector.size(); i++)
      delete MenuOptionsVector[i];
  }
};

extern int menuindex;
extern int currentmenusize;
extern PauseMenu* currentpausemenu;