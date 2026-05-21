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
extern std::vector<std::unique_ptr<MenuOptions>> MenuOptionsVector;

void clearMenuOptionsVector();
