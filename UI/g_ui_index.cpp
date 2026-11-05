#include "extern.h"
#include "g_ui.h"
#include "p_main.h"
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

int menuindex = 0;
std::vector<std::unique_ptr<MenuOptions>> MenuOptionsVector;

void UIupdate() {
  if (LocalInputs->Keys[SDL_SCANCODE_RETURN] == 2) {
    MenuOptionsVector[menuindex]->Interact();
  }
  if (LocalInputs->Keys[SDL_SCANCODE_W] == 2) {
    menuindex--;
    if (menuindex < 0) menuindex = MenuOptionsVector.size() - 1;
  }
  if (LocalInputs->Keys[SDL_SCANCODE_S] == 2) {
    menuindex++;
    if (menuindex >= MenuOptionsVector.size()) menuindex = 0;
  }
}

UI_API void changeUIindex() {
  if (Global->pause) {
    if (MenuOptionsVector.empty() && !UIGlobalStuff->UImap.contains("Menus") &&
        UIGlobalStuff->UIlist.back() != "Menus") {
      UIGlobalStuff->UIlist.clear();
      UIGlobalStuff->UIlist.push_back("Pause");
      UIGlobalStuff->UIlist.push_back("Menus");
      UIGlobalStuff->UImap["Menus"];

      MenuOptionsVector.push_back(std::make_unique<ResumeMenuOption>(0));
      MenuOptionsVector.push_back(std::make_unique<SettingsMenuOption>(1));
      MenuOptionsVector.push_back(std::make_unique<QuitMenuOption>(2));
    }
    UIupdate();

  } else {
    menuindex = 0;
    if (!MenuOptionsVector.empty() || UIGlobalStuff->UImap.contains("Menus") ||
        UIGlobalStuff->UIlist.back() == "Menus") {
      clearMenuOptionsVector();
    }
    if (UIGlobalStuff->UIlist.front() != "CobblerGameBaseUI") {
      UIGlobalStuff->UIlist.clear();
      UIGlobalStuff->UIlist.push_back("CobblerGameBaseUI");
      UIGlobalStuff->UIlist.push_back("Gardner/AmmoCnt");
      UIGlobalStuff->UIlist.push_back("Gardner/Weapon0");
    }
    UIGlobalStuff->UIlist.back() = "Gardner/Weapon0";
  }
}