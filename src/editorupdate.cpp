#include <cmath>

#include "entity.h"
#include "extern.h"
#include "files.h"
#include "global.h"
#include "update.h"

void movecamera() {
  if (P1Inputs->rightclick > 0) {
    Camera->dir.x += -0.5f * P1Inputs->MousePos.x;
    Camera->dir.y += -0.5f * P1Inputs->MousePos.y;
  }

  float ps = std::sin(Camera->dir.x * 3.14 / 180.0);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.0);
  float whats = std::sin(Camera->dir.y * 3.14 / 180.0);
  float whatc = std::cos(Camera->dir.y * 3.14 / 180.0);

  Camera->position.x -=
      P1Inputs->MouseScroll.x * std::sin((Camera->dir.x - 90) * 3.14 / 180.0);
  Camera->position.y +=
      P1Inputs->MouseScroll.x * std::cos((Camera->dir.x - 90) * 3.14 / 180.0);
  Camera->position.x += P1Inputs->MouseScroll.y * ps * whatc;
  Camera->position.y -= P1Inputs->MouseScroll.y * pc * whatc;
  Camera->position.z -= P1Inputs->MouseScroll.y * whats;

  P1Inputs->MousePos.x = 0;
  P1Inputs->MousePos.y = 0;
  P1Inputs->MouseScroll.x = std::lerp(P1Inputs->MouseScroll.x, 0, 0.5f);
  P1Inputs->MouseScroll.y = std::lerp(P1Inputs->MouseScroll.y, 0, 0.5f);
  if (Camera->dir.x < 0) Camera->dir.x += 360;
  if (Camera->dir.x >= 360) Camera->dir.x -= 360;
  if (Camera->dir.y >= 90) Camera->dir.y = 90;
  if (Camera->dir.y <= -90) Camera->dir.y = -90;

  if (P1Inputs->S == 2) {
    savemap();
  }
}

void update() {
  if (P1Inputs->ESC == 2) {
    Global->pause = !Global->pause;
  }
  if (!Global->pause) movecamera();
}