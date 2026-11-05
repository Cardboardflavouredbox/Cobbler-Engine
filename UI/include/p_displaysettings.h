#pragma once
#include "files.h"
#include "p_global.h"
#include "pausemenu.h"
#include "render.h"
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

struct UI_API VsyncOption : CheckBoxMenuOption {
  void Interact() {
    Settings->vsync = !Settings->vsync;
    switch (Settings->graphicsmode) {
      case Vulkan: {
        break;
      }
      case OpenGL4:
      case OpenGL3:
      case OpenGL1: {  // opengl
        SDL_GL_SetSwapInterval(Settings->vsync ? 1 : 0);
        break;
      }
      case Software: {  // software
        SDL_SetRenderVSync(RendererGlobal->SRstuff->renderer,
                           Settings->vsync ? 1 : SDL_RENDERER_VSYNC_DISABLED);

        break;
      }
    }
  }
  VsyncOption(int i) : CheckBoxMenuOption(&Settings->vsync) {
    name = U"Vsync";
    index = i;
    UIcomponent->pos = glm::vec2(16, 12 * (index + 1));
  }
  ~VsyncOption() {}
};

UI_API void setMenuDisplaySettings() {
  menuindex = 0;
  clearMenuOptionsVector();
  UIGlobalStuff->UIlist.push_back("Menus");
  UIGlobalStuff->UImap["Menus"];
  MenuOptionsVector.push_back(std::make_unique<VsyncOption>(0));
}