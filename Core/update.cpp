#include "update.h"

#include <SDL3/SDL_timer.h>

#include <cmath>
#include <glm/glm.hpp>

#include "components.h"
#include "entity.h"
#include "extern.h"
#include "global.h"
#include "network.h"
#include "physics.h"

void playermovement() {
  Camera->dir.x += -1 * Settings->mousesensitivity.x * P1Inputs->MouseDelta.x;
  Camera->dir.y += -1 * Settings->mousesensitivity.y * P1Inputs->MouseDelta.y;
  P1Inputs->MouseDelta.x = 0;
  P1Inputs->MouseDelta.y = 0;

  float ps = std::sin(Camera->dir.x * PI / 180.0);
  float pc = std::cos(Camera->dir.x * PI / 180.0);

  glm::vec3 tempmove = glm::vec3({0, 0, 0});
  if (P1Inputs->A > 0 && P1Inputs->D == 0) {
    tempmove.x -= std::sin((Camera->dir.x + 90) * PI / 180.0);
    tempmove.y += std::cos((Camera->dir.x + 90) * PI / 180.0);
  }
  if (P1Inputs->D > 0 && P1Inputs->A == 0) {
    tempmove.x -= std::sin((Camera->dir.x - 90) * PI / 180.0);
    tempmove.y += std::cos((Camera->dir.x - 90) * PI / 180.0);
  }
  if (P1Inputs->W > 0) {
    tempmove.x -= ps;
    tempmove.y += pc;
  }
  if (P1Inputs->S > 0) {
    tempmove.x += ps;
    tempmove.y -= pc;
  }

  if (tempmove != glm::vec3(0)) tempmove = glm::normalize(tempmove);

  Camera->movevec3 = (tempmove * Camera->walkspeed);

  if ((P1Inputs->Shift == 0 && Settings->autorun) ||
      (P1Inputs->Shift > 0 && !Settings->autorun))
    Camera->movevec3 = (Camera->movevec3 * Camera->runspeed);

  if (P1Inputs->Space == 2 && Camera->IsGrounded)
    Camera->velocityvec3.z = Camera->jumpheight;

  if (Camera->dir.x < 0) Camera->dir.x += 360;
  if (Camera->dir.x >= 360) Camera->dir.x -= 360;
  if (Camera->dir.y >= 89) Camera->dir.y = 89;
  if (Camera->dir.y <= -89) Camera->dir.y = -89;

  if (P1Inputs->leftclick == 2) {
    float ps = std::sin(Camera->dir.x * PI / 180.f);
    float pc = std::cos(Camera->dir.x * PI / 180.f);
    float what = std::cos(Camera->dir.y * PI / 180.f);
    glm::vec3 ray[2];
    ray[0] = Camera->position;
    ray[1] = {-ps * what, pc * what, std::sin(Camera->dir.y * PI / 180.f)};
    ray[1] *= 32.f;
    ray[1] += Camera->position;
    for (auto& i : Global->Entities) {
      if (i->teamindex != Camera->teamindex &&
          capsuleraycheck(ray[0], ray[1], i->hitbox[0] + i->position,
                          i->hitbox[1] + i->position, i->hitboxradius)) {
        i->velocityvec3.z = 25;
        curlpostfield->Kills++;
        curlpostfield->hasdata = true;
      }
    }
  }
}

void update() {
  lastTime = currentTime;
  currentTime = SDL_GetPerformanceCounter();
  Global->deltaTime =
      ((currentTime - lastTime) * 10) / (double)SDL_GetPerformanceFrequency();

  if (P1Inputs->ESC == 2) {
    Global->pause = !Global->pause;
    SDL_SetWindowRelativeMouseMode(Global->window, !Global->pause);
  }
  if (!Global->pause) {
    playermovement();
    componentsupdate();
  }
  componentsupdatelate();

  if (curlpostfield->hasdata) CobblerSendCurlData();
}