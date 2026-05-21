#include "g_ui.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>

#include "extern.h"
#include "pausemenu.h"
#include "ui.h"

void clearMenuOptionsVector() {
  MenuOptionsVector.clear();
  Global->UImap.erase("Menu");
  Global->UIlist.pop_back();
}

// UI setup
bool UIsetup() {
  Global->UImap.reserve(4);
  Global->UImap["Pistol"];
  UIimage* weapon = new UIimage();
  weapon->color = 11;
  weapon->rgba = glm::vec4(1, 1, 1, 1);
  weapon->pos = glm::vec2({160 - 48, 200 - 112});
  weapon->size = glm::vec2({96, 112});
  weapon->uvlist =
      new std::pair<glm::vec2, glm::vec2>[3]{{{0, 0}, {1 / 3.f, 1.f}},
                                             {{1 / 3.f, 0}, {2 / 3.f, 1.f}},
                                             {{2 / 3.f, 0}, {1.f, 1.f}}};
  ImagePistolChanger* IPC = new ImagePistolChanger();
  IPC->index = &weapon->uvindex;

  weapon->UVIndexChanger = IPC;
  weapon->texturename = "Pistol";
  Global->UImap["Pistol"].push_back(weapon);

  UIbox* crosshair = new UIbox();
  crosshair->rgba = glm::vec4(1, 1, 1, 1);
  crosshair->color = 11;
  crosshair->size = glm::vec2({2, 2});
  crosshair->pos = glm::vec2({159, 99});

  Global->UImap["Pistol"].push_back(crosshair);

  Global->UImap["Pause"];
  UIbox* background = new UIbox();
  background->rgba = glm::vec4(0, 0, 0, 1);
  background->color = 0;
  background->size = glm::vec2({320, 200});
  background->pos = glm::vec2({0, 0});
  Global->UImap["Pause"].push_back(background);

  return true;
}

void UIfree() { clearMenuOptionsVector(); }