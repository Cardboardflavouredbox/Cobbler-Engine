#include "update.h"

#include <SDL3/SDL.h>

#include <cmath>

#include "entity.h"
#include "extern.h"
#include "global.h"

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

  SDL_GetRelativeMouseState(&P1Inputs->Mouse.x, &P1Inputs->Mouse.y);
  Camera->dir.x += -4 * P1Inputs->Mouse.x;
  P1Inputs->Mouse.x = 0;
  P1Inputs->Mouse.y = 0;

  float ps = std::sin(Camera->dir.x * 3.14 / 180.0);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.0);

  if (P1Inputs->A > 0) {
    Camera->position.x -=
        8 * Global->deltaTime * std::sin((Camera->dir.x + 90) * 3.14 / 180.0);
    Camera->position.y +=
        8 * Global->deltaTime * std::cos((Camera->dir.x + 90) * 3.14 / 180.0);
  }
  if (P1Inputs->D > 0) {
    Camera->position.x -=
        8 * Global->deltaTime * std::sin((Camera->dir.x - 90) * 3.14 / 180.0);
    Camera->position.y +=
        8 * Global->deltaTime * std::cos((Camera->dir.x - 90) * 3.14 / 180.0);
  }
  if (P1Inputs->W > 0) {
    Camera->position.x -= 8 * Global->deltaTime * ps;
    Camera->position.y += 8 * Global->deltaTime * pc;
  }
  if (P1Inputs->S > 0) {
    Camera->position.x += 8 * Global->deltaTime * ps;
    Camera->position.y -= 8 * Global->deltaTime * pc;
  }
  if (Camera->dir.x >= 360) Camera->dir.x -= 360;
}