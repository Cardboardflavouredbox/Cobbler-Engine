#pragma once
#include <memory>
#include <vector>

#include "ui.h"

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

struct LIB_API MenuOptions {
  UIthing* UIcomponent;
  std::u32string name;
  int index;
  virtual void Interact() = 0;
  virtual ~MenuOptions() {}
};

LIB_API extern int menuindex;
LIB_API extern std::vector<std::unique_ptr<MenuOptions>> MenuOptionsVector;

LIB_API void clearMenuOptionsVector();
