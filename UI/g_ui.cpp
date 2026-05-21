#include "g_ui.h"

#include <SDL3/SDL_log.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>

#include "extern.h"
#include "pausemenu.h"
#include "ui.h"

void clearMenuOptionsVector() {
  MenuOptionsVector.clear();
  if (Global->UImap.contains("Menus")) {
    while (!Global->UImap["Menus"].empty()) {
      delete (Global->UImap["Menus"].back());
      Global->UImap["Menus"].pop_back();
    }
    Global->UImap.erase("Menus");
  }
  while (Global->UIlist.back() == "Menus") Global->UIlist.pop_back();
}

// UI setup
bool UIsetup() {
  Global->UImap.reserve(4);
  Global->UImap["Pistol"];
  UIimage* weapon = new UIimage();
  weapon->color = 11;
  weapon->rgba = glm::vec4(1, 1, 1, 1);
  weapon->pos =
      glm::vec2({Settings->resolutionx - 142, Settings->resolutiony - 181});
  weapon->size = glm::vec2({142, 181});
  weapon->uvlist =
      new std::pair<glm::vec2, glm::vec2>[3]{{{0, 0}, {1 / 2.f, 1.f}},
                                             {{0, 0}, {1 / 2.f, 1.f}},
                                             {{1 / 2.f, 0}, {1.f, 1.f}}};
  ImagePistolChanger* IPC = new ImagePistolChanger();
  IPC->index = &weapon->uvindex;

  weapon->UVIndexChanger = IPC;
  weapon->texturename = "Pistol";
  Global->UImap["Pistol"].push_back(weapon);

  UIbox* crosshair = new UIbox();
  crosshair->rgba = glm::vec4(1, 1, 1, 1);
  crosshair->color = 11;
  crosshair->size = glm::vec2({2, 2});
  crosshair->pos =
      glm::vec2({Settings->resolutionx / 2 - 1, Settings->resolutiony / 2 - 1});

  Global->UImap["Pistol"].push_back(crosshair);

  Global->UImap["Pause"];
  UIbox* background = new UIbox();
  background->rgba = glm::vec4(0, 0, 0, 1);
  background->color = 255;
  background->size = glm::vec2({Settings->resolutionx, Settings->resolutiony});
  background->pos = glm::vec2({0, 0});
  Global->UImap["Pause"].push_back(background);

  return true;
}

void UIfree() { clearMenuOptionsVector(); }