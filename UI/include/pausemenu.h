#pragma once
#include <vector>

struct MenuOptions {
  UIthing* UIcomponent;
  std::string name;
  int index;
  virtual void Interact() = 0;
  virtual ~MenuOptions() {}
};

extern int menuindex;
extern std::vector<MenuOptions*> MenuOptionsVector;

// for (int i = 0; i < MenuOptionsVector.size(); i++) delete
// MenuOptionsVector[i];