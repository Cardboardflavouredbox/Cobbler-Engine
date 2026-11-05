#pragma once

#include "commandline.h"
#include "g_ui.h"

#ifdef _WIN32
#ifdef UIEXPORT
#define UI_API __declspec(dllexport)
#else
#define UI_API __declspec(dllimport)
#endif
#else
#define UI_API
#endif

struct UI_API MenuCMDtextChanger : UITextChanger {
  void update() {
    std::string str = cmdlines.str();
    std::u32string s32(str.begin(), str.end());
    *string = s32;
  }
  MenuCMDtextChanger() {}
  ~MenuCMDtextChanger() {}
};

struct UI_API MenuCMDInputtextChanger : UITextChanger {
  void update() {
    std::u32string s32(cmdcurrentinput.begin(), cmdcurrentinput.end());
    *string = s32;
  }
  MenuCMDInputtextChanger() {}
  ~MenuCMDInputtextChanger() {}
};
