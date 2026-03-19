#include "update.h"

#include <SDL3/SDL.h>

#include <cmath>

#include "entity.h"
#include "extern.h"
#include "global.h"

Vector3 closestPointTriangle(Vector3 p, Vector3 a, Vector3 b, Vector3 c) {
  const Vector3 ab = subVec3(b, a);
  const Vector3 ac = subVec3(c, a);
  const Vector3 ap = subVec3(p, a);

  const float d1 = Vector3Dot(ab, ap);
  const float d2 = Vector3Dot(ac, ap);
  if (d1 <= 0.f && d2 <= 0.f) return a;  // #1

  const Vector3 bp = subVec3(p, b);
  const float d3 = Vector3Dot(ab, bp);
  const float d4 = Vector3Dot(ac, bp);
  if (d3 >= 0.f && d4 <= d3) return b;  // #2

  const Vector3 cp = subVec3(p, c);
  const float d5 = Vector3Dot(ab, cp);
  const float d6 = Vector3Dot(ac, cp);
  if (d6 >= 0.f && d5 <= d6) return c;  // #3

  const float vc = d1 * d4 - d3 * d2;
  if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f) {
    const float v = d1 / (d1 - d3);
    return addVec3(a, multiplyVec3(ab, v));  // #4
  }

  const float vb = d5 * d2 - d1 * d6;
  if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f) {
    const float v = d2 / (d2 - d6);
    return addVec3(a, multiplyVec3(ac, v));  // #5
  }

  const float va = d3 * d6 - d5 * d4;
  if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f) {
    const float v = (d4 - d3) / ((d4 - d3) + (d5 - d6));
    return addVec3(b, multiplyVec3(subVec3(c, b), v));  // #6
  }

  const float denom = 1.f / (va + vb + vc);
  const float v = vb * denom;
  const float w = vc * denom;
  return addVec3(a, addVec3(multiplyVec3(ab, v), multiplyVec3(ac, w)));  // #0
}

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

  Camera->moveVector3 = multiplyVec3(tempmove, 8);

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
      Vector3 tempmove = multiplyVec3(
          addVec3(tempentity->moveVector3, tempentity->velocityVector3),
          Global->deltaTime);
      tempentity->position = addVec3(tempentity->position, tempmove);
      tempentity->moveVector3 = Vector3({0, 0, 0});
    }
  }
}