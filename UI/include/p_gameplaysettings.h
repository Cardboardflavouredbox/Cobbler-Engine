#pragma once
#include "files.h"
#include "p_global.h"
#include "pausemenu.h"
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

// struct LIB_API AutorunOption : CheckBoxMenuOption {
//   void Interact() { Settings->autorun = !Settings->autorun; }
//   AutorunOption(int i) : CheckBoxMenuOption(&Settings->autorun) {
//     name = U"Autorun";
//     index = i;
//     UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
//   }
//   ~AutorunOption() {}
// };

LIB_API void setMenuGameplaySettings() {
  menuindex = 0;
  clearMenuOptionsVector();
  UIGlobalStuff->UIlist.push_back("Menus");
  UIGlobalStuff->UImap["Menus"];
  // MenuOptionsVector.push_back(std::make_unique<AutorunOption>(0));
}