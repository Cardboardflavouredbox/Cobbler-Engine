#include <SDL3/SDL.h>

#include <cmath>
#include <glm/glm.hpp>

#include "components.h"
#include "entity.h"
#include "extern.h"
#include "global.h"
#include "ui_index.h"
#include "update.h"

void playermovement() {
  Camera->dir.x += -2 * P1Inputs->MouseDelta.x;
  Camera->dir.y += -2 * P1Inputs->MouseDelta.y;
  P1Inputs->MouseDelta.x = 0;
  P1Inputs->MouseDelta.y = 0;

  float ps = std::sin(Camera->dir.x * PI / 180.0);
  float pc = std::cos(Camera->dir.x * PI / 180.0);

  glm::vec3 tempmove = glm::vec3({0, 0, 0});
  if (P1Inputs->A > 0) {
    tempmove.x -= std::sin((Camera->dir.x + 90) * PI / 180.0);
    tempmove.y += std::cos((Camera->dir.x + 90) * PI / 180.0);
  }
  if (P1Inputs->D > 0) {
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

  tempmove = glm::normalize(tempmove);

  Camera->movevec3 = (tempmove * Camera->walkspeed);

  if (P1Inputs->Shift > 0)
    Camera->movevec3 = (Camera->movevec3 * Camera->runspeed);

  if (P1Inputs->Space == 2 && Camera->IsGrounded)
    Camera->velocityvec3.z = Camera->jumpheight;

  if (Camera->dir.x < 0) Camera->dir.x += 360;
  if (Camera->dir.x >= 360) Camera->dir.x -= 360;
  if (Camera->dir.y >= 90) Camera->dir.y = 90;
  if (Camera->dir.y <= -90) Camera->dir.y = -90;
}

void update() {
  lastTime = currentTime;
  currentTime = SDL_GetPerformanceCounter();
  Global->deltaTime =
      (currentTime - lastTime) / (double)SDL_GetPerformanceFrequency();

  if (P1Inputs->ESC == 2) {
    Global->pause = !Global->pause;
    SDL_SetWindowRelativeMouseMode(Global->window, !Global->pause);
  }
  if (!Global->pause) {
    playermovement();
    componentsupdate();
    changeUIindex();
    componentsupdatelate();
  }
}