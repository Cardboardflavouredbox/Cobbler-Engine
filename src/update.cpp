#include "update.h"

#include <SDL3/SDL.h>

#include <cmath>

#include "entity.h"
#include "extern.h"
#include "global.h"

void playermovement() {
  SDL_GetRelativeMouseState(&P1Inputs->Mouse.x, &P1Inputs->Mouse.y);
  Camera->dir.x += -2 * P1Inputs->Mouse.x;
  Camera->dir.y += -2 * P1Inputs->Mouse.y;
  P1Inputs->Mouse.x = 0;
  P1Inputs->Mouse.y = 0;

  float ps = std::sin(Camera->dir.x * 3.14 / 180.0);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.0);

  Vector3 tempmove = Vector3({0, 0, 0});
  if (P1Inputs->A > 0) {
    tempmove.x -= std::sin((Camera->dir.x + 90) * 3.14 / 180.0);
    tempmove.y += std::cos((Camera->dir.x + 90) * 3.14 / 180.0);
  }
  if (P1Inputs->D > 0) {
    tempmove.x -= std::sin((Camera->dir.x - 90) * 3.14 / 180.0);
    tempmove.y += std::cos((Camera->dir.x - 90) * 3.14 / 180.0);
  }
  if (P1Inputs->W > 0) {
    tempmove.x -= ps;
    tempmove.y += pc;
  }
  if (P1Inputs->S > 0) {
    tempmove.x += ps;
    tempmove.y -= pc;
  }

  tempmove = Vector3Normalize(tempmove);

  Camera->moveVector3 =
      multiplyVec3(multiplyVec3(tempmove, 8), Global->deltaTime);

  if (P1Inputs->Shift > 0)
    Camera->moveVector3 = multiplyVec3(Camera->moveVector3, 1.75f);

  if (Camera->dir.x >= 360) Camera->dir.x -= 360;
  if (Camera->dir.y >= 90) Camera->dir.y = 90;
  if (Camera->dir.y <= -90) Camera->dir.y = -90;
}

void update() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        Global->IsRunning = false;
        break;
    }
  }
  Uint32 currentTime = SDL_GetTicks();
  Global->deltaTime = (currentTime - lastTime) / 1000.0f;
  lastTime = currentTime;

  if (P1Inputs->ESC == 2) {
    Global->pause = !Global->pause;
    SDL_SetWindowRelativeMouseMode(Global->window, !Global->pause);
  }
  if (!Global->pause) {
    playermovement();
    for (int i = 0; i < Global->Entities.size(); i++) {
      Entity* tempentity = Global->Entities[i];
      tempentity->position =
          addVec3(tempentity->position, tempentity->moveVector3);
    }
  }
}