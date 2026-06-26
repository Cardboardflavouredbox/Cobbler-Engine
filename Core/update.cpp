#include "update.h"

#include <SDL3/SDL_timer.h>

#include <cmath>
#include <glm/glm.hpp>

#include "components.h"
#include "entity.h"
#include "extern.h"
#include "global.h"
#include "network.h"
#include "player.h"

void processinputs() {
  P1PlayerInputs->lookdir.x +=
      -1 * Settings->mousesensitivity.x * LocalInputs->MouseDelta.x;
  P1PlayerInputs->lookdir.y +=
      -1 * Settings->mousesensitivity.y * LocalInputs->MouseDelta.y;

  if (P1PlayerInputs->lookdir.x < 0) P1PlayerInputs->lookdir.x += 360;
  if (P1PlayerInputs->lookdir.x >= 360) P1PlayerInputs->lookdir.x -= 360;
  if (P1PlayerInputs->lookdir.y >= 89) P1PlayerInputs->lookdir.y = 89;
  if (P1PlayerInputs->lookdir.y <= -89) P1PlayerInputs->lookdir.y = -89;

  LocalInputs->MouseDelta.x = 0;
  LocalInputs->MouseDelta.y = 0;
  float ps = std::sin(P1PlayerInputs->lookdir.x * PI / 180.0);
  float pc = std::cos(P1PlayerInputs->lookdir.x * PI / 180.0);

  glm::vec3 tempmove = glm::vec3({0, 0, 0});
  if (LocalInputs->A > 0 && LocalInputs->D == 0) {
    tempmove.x -= std::sin((P1PlayerInputs->lookdir.x + 90) * PI / 180.0);
    tempmove.y += std::cos((P1PlayerInputs->lookdir.x + 90) * PI / 180.0);
  }
  if (LocalInputs->D > 0 && LocalInputs->A == 0) {
    tempmove.x -= std::sin((P1PlayerInputs->lookdir.x - 90) * PI / 180.0);
    tempmove.y += std::cos((P1PlayerInputs->lookdir.x - 90) * PI / 180.0);
  }
  if (LocalInputs->W > 0) {
    tempmove.x -= ps;
    tempmove.y += pc;
  }
  if (LocalInputs->S > 0) {
    tempmove.x += ps;
    tempmove.y -= pc;
  }

  if (tempmove != glm::vec3(0)) tempmove = glm::normalize(tempmove);
  P1PlayerInputs->movevec3 = tempmove;
  P1PlayerInputs->jump = LocalInputs->Space;
}

void update() {
  lastTime = currentTime;
  currentTime = SDL_GetPerformanceCounter();
  Global->deltaTime =
      ((currentTime - lastTime) * 10) / (double)SDL_GetPerformanceFrequency();

  if (LocalInputs->ESC == 2) {
    Global->pause = !Global->pause;
    SDL_SetWindowRelativeMouseMode(Global->window, !Global->pause);
  }

  if (!Global->pause) {
    processinputs();
    inputtoentity(*P1PlayerInputs, Camera);
    PlayerClassUpdate[Global->playerclass]();
    componentsupdate();
    for (auto& [key, value] : Global->UImap3D) {
      for (auto& i : value) {
        i->update();
      }
    }
  }
  componentsupdatelate();

  if (curlpostfield->hasdata && Global->LoggedIn) CobblerSendCurlData();
}