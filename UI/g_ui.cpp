#include "g_ui.h"

#include <SDL3/SDL_log.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>

#include "extern.h"
#include "pausemenu.h"
#include "ui.h"

glm::vec2 getrealpos(UIthing::anchorpos anchor, glm::vec2 pos, glm::vec2 size) {
  glm::vec2 realpos;
  switch (anchor % 3) {
    case 0:
      realpos.x = pos.x;
      break;
    case 1:
      realpos.x = Settings->resolutionx / 2 - size.x / 2 + pos.x;
      break;
    case 2:
      realpos.x = Settings->resolutionx + pos.x;
      break;
  }
  switch (anchor / 3) {
    case 0:
      realpos.y = pos.y;
      break;
    case 1:
      realpos.y = Settings->resolutiony / 2 - size.y / 2 + pos.y;
      break;
    case 2:
      realpos.y = Settings->resolutiony + pos.y;
      break;
  }
  return realpos;
}

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
  weapon->anchor = UIthing::rightbottom;
  weapon->color = 11;
  weapon->rgba = glm::vec4(1, 1, 1, 1);
  weapon->pos = glm::vec2({-142, -181});
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
  crosshair->anchor = UIthing::middlemiddle;
  crosshair->rgba = glm::vec4(1, 1, 1, 1);
  crosshair->color = 11;
  crosshair->size = glm::vec2({2, 2});
  crosshair->pos = glm::vec2({0, 0});

  Global->UImap["Pistol"].push_back(crosshair);

  Global->UImap["Pause"];
  UIbox* background = new UIbox();
  background->anchor = UIthing::lefttop;
  background->rgba = glm::vec4(0, 0, 0, 1);
  background->color = 255;
  background->size = glm::vec2({Settings->resolutionx, Settings->resolutiony});
  background->pos = glm::vec2({0, 0});
  Global->UImap["Pause"].push_back(background);

  return true;
}

void UIfree() { clearMenuOptionsVector(); }