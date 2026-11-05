#pragma once
#include "files.h"
#include "p_global.h"
#include "pausemenu.h"
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

// struct UI_API AutorunOption : CheckBoxMenuOption {
//   void Interact() { Settings->autorun = !Settings->autorun; }
//   AutorunOption(int i) : CheckBoxMenuOption(&Settings->autorun) {
//     name = U"Autorun";
//     index = i;
//     UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
//   }
//   ~AutorunOption() {}
// };

UI_API void setMenuGameplaySettings() {
  menuindex = 0;
  clearMenuOptionsVector();
  UIGlobalStuff->UIlist.push_back("Menus");
  UIGlobalStuff->UImap["Menus"];
  // MenuOptionsVector.push_back(std::make_unique<AutorunOption>(0));
}