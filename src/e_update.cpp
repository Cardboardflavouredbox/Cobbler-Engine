#include <SDL3/SDL_log.h>

#include <cmath>
#include <glm/glm.hpp>

#include "components.h"
#include "entity.h"
#include "extern.h"
#include "files.h"
#include "global.h"
#include "raycast.h"
#include "screen.h"
#include "ui_index.h"
#include "update.h"

glm::vec2 MouseToScreenpos() {
  int x, y, w = Global->windowx, h = Global->windowy,
            rtw = Global->render_target->w, rth = Global->render_target->h;
  int size = w / rtw;
  if (size > h / rth) size = h / rth;

  rtw *= size;
  rth *= size;

  w /= 2;
  h /= 2;
  w -= rtw / 2;
  h -= rth / 2;

  x = (P1Inputs->MousePos.x - w) / size;
  y = (P1Inputs->MousePos.y - h) / size;
  return glm::vec2({(float)x, (float)y});
}

bool ScreenPointMouseDetect(ScreenPoint SP) {
  int x, y, w = Global->windowx, h = Global->windowy,
            rtw = Global->render_target->w, rth = Global->render_target->h;
  int size = w / rtw;
  if (size > h / rth) size = h / rth;

  rtw *= size;
  rth *= size;

  w /= 2;
  h /= 2;
  w -= rtw / 2;
  h -= rth / 2;

  x = (P1Inputs->MousePos.x - w) / size;
  y = (P1Inputs->MousePos.y - h) / size;

  if (std::abs(x - SP.p.x) <= 2 && std::abs(y - SP.p.y) <= 2) {
    return true;
  }
  return false;
}

void noncamerastuff() {
  if (P1Inputs->LCTRL > 0 && P1Inputs->S == 2) {
    savemap();
  }
}

void movecamera() {
  if (P1Inputs->rightclick > 0) {
    Camera->dir.x += -0.5f * P1Inputs->MouseDelta.x;
    Camera->dir.y += -0.5f * P1Inputs->MouseDelta.y;
  }

  if (P1Inputs->numkeys[0] == 2) {
    Global->rendermode = 0;
  } else if (P1Inputs->numkeys[1] == 2) {
    Global->rendermode = 1;
  }

  float ps = std::sin(Camera->dir.x * PI / 180.0);
  float pc = std::cos(Camera->dir.x * PI / 180.0);
  float whats = std::sin(Camera->dir.y * PI / 180.0);
  float whatc = std::cos(Camera->dir.y * PI / 180.0);

  Camera->position.x -=
      P1Inputs->MouseScroll.x * std::sin((Camera->dir.x - 90) * PI / 180.0);
  Camera->position.y +=
      P1Inputs->MouseScroll.x * std::cos((Camera->dir.x - 90) * PI / 180.0);
  Camera->position.x -= P1Inputs->MouseScroll.y * ps * whatc;
  Camera->position.y += P1Inputs->MouseScroll.y * pc * whatc;
  Camera->position.z += P1Inputs->MouseScroll.y * whats;

  P1Inputs->MouseDelta.x = 0;
  P1Inputs->MouseDelta.y = 0;
  P1Inputs->MouseScroll.x = std::lerp(P1Inputs->MouseScroll.x, 0, 0.5f);
  P1Inputs->MouseScroll.y = std::lerp(P1Inputs->MouseScroll.y, 0, 0.5f);
  if (Camera->dir.x < 0) Camera->dir.x += 360;
  if (Camera->dir.x >= 360) Camera->dir.x -= 360;
  if (Camera->dir.y >= 90) Camera->dir.y = 90;
  if (Camera->dir.y <= -90) Camera->dir.y = -90;
}

void update() {
  if (!Global->isopeningfile) {
    if (P1Inputs->ESC == 2) {
      Global->pause = !Global->pause;
    }
    if (!Global->pause) {
      noncamerastuff();
      movecamera();
      changeUIindex();
      componentsupdatelate();
    }
  }
}