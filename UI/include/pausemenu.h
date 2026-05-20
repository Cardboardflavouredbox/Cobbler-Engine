#pragma once
#include <vector>

struct MenuOptions {
  UIthing* UIcomponent;
  std::string name;
  int index;
  virtual void Interact() = 0;
  virtual ~MenuOptions() { delete UIcomponent; }
};

extern int menuindex;
extern std::vector<MenuOptions*> MenuOptionsVector;

void clearMenuOptionsVector();
