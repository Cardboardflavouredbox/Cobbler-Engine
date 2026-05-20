#pragma once
#include <vector>

struct MenuOptions {
  std::shared_ptr<UIthing> UIcomponent;
  std::string name;
  int index;
  virtual void Interact() = 0;
  virtual ~MenuOptions() {}
};

extern int menuindex;
extern std::vector<std::shared_ptr<MenuOptions>> MenuOptionsVector;

void clearMenuOptionsVector();
