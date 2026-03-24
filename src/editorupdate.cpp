#include <cmath>

#include "entity.h"
#include "extern.h"
#include "files.h"
#include "global.h"
#include "screen.h"
#include "update.h"

void movecamera() {
  if (Global->rendermode == 1) {
    if (P1Inputs->leftclick > 1) {
      for (int i = 0; i < Global->Points.size(); i++) {
        ScreenPoint ScreenSpacePoint = ToScreenSpace(Global->Points[i]);

        int x, y, w = Global->windowx, h = Global->windowy,
                  rtw = Global->render_target->w,
                  rth = Global->render_target->h;
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

        if (std::abs(x - ScreenSpacePoint.p.x) <= 2 &&
            std::abs(y - ScreenSpacePoint.p.y) <= 2) {
          Global->editorselectedPoint = i;
        }
      }
    }
  }
  if (P1Inputs->rightclick > 0) {
    Camera->dir.x += -0.5f * P1Inputs->MouseDelta.x;
    Camera->dir.y += -0.5f * P1Inputs->MouseDelta.y;
  }

  if (P1Inputs->numkeys[0] == 2) {
    Global->rendermode = 0;
  } else if (P1Inputs->numkeys[1] == 2) {
    Global->rendermode = 1;
  }

  float ps = std::sin(Camera->dir.x * 3.14 / 180.0);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.0);
  float whats = std::sin(Camera->dir.y * 3.14 / 180.0);
  float whatc = std::cos(Camera->dir.y * 3.14 / 180.0);

  Camera->position.x -=
      P1Inputs->MouseScroll.x * std::sin((Camera->dir.x - 90) * 3.14 / 180.0);
  Camera->position.y +=
      P1Inputs->MouseScroll.x * std::cos((Camera->dir.x - 90) * 3.14 / 180.0);
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

  if (P1Inputs->LCTRL > 0 && P1Inputs->S == 2) {
    savemap();
  }
}

void update() {
  if (!Global->isopeningfile) {
    if (P1Inputs->ESC == 2) {
      Global->pause = !Global->pause;
    }
    if (!Global->pause) movecamera();
  }
}