#pragma once
#include <memory>
#include <vector>

#include "ui.h"

#ifdef _WIN32
#ifdef UIEXPORT
#define UI_API __declspec(dllexport)
#else
#define UI_API __declspec(dllimport)
#endif
#else
#define UI_API
#endif

struct UI_API MenuOptions {
  UIthing* UIcomponent;
  std::u32string name;
  int index;
  virtual void Interact() = 0;
  virtual ~MenuOptions() {}
};

UI_API extern int menuindex;
UI_API extern std::vector<std::unique_ptr<MenuOptions>> MenuOptionsVector;

UI_API void clearMenuOptionsVector();
