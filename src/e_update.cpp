#include <SDL3/SDL_log.h>

#include <cmath>
#include <glm/glm.hpp>

#include "components.h"
#include "entity.h"
#include "extern.h"
#include "files.h"
#include "global.h"
#include "raycast.h"
#include "rendermath.h"
#include "screen.h"
#include "ui_index.h"
#include "update.h"

glm::vec2 MouseToScreenpos() {
  int x, y, w = Global->windowx, h = Global->windowy,
            rtw = Settings->resolutionx, rth = Settings->resolutiony;
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

ScreenPoint ToScreenSpace(glm::vec3 P) {
  glm::vec2 temp = (glm::vec2)P - Editor->pos;
  temp *= Editor->zoom;
  temp.x *= -1;
  temp.x += (Settings->resolutionx / 2);
  temp.y += (Settings->resolutiony / 2);
  ScreenPoint sp;
  sp.dist = 32;
  sp.isbehindcamera = false;
  sp.p = temp;
  return sp;
}

glm::vec3 ToWorldSpace(glm::vec2 P) {
  P.x -= (Settings->resolutionx / 2);
  P.y -= (Settings->resolutiony / 2);
  P.x *= -1;
  P /= Editor->zoom;
  P += Editor->pos;
  return glm::vec3(P.x, P.y, 0);
}

bool ScreenPointMouseDetect(ScreenPoint SP) {
  int x, y, w = Global->windowx, h = Global->windowy,
            rtw = Settings->resolutionx, rth = Settings->resolutiony;
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
    if (Global->pause) {
      for (int i = 0; i < Global->Points.size(); i++) {
        if (ScreenPointMouseDetect(ToScreenSpace(Global->Points[i]))) {
          Global->Points.erase(Global->Points.begin() + i);
        }
      }
    } else {
      int w = Global->windowx, h = Global->windowy, rtw = Settings->resolutionx,
          rth = Settings->resolutiony;
      int size = w / rtw;
      if (size > h / rth) size = h / rth;
      Editor->pos.x += P1Inputs->MouseDelta.x / Editor->zoom / (float)size;
      Editor->pos.y -= P1Inputs->MouseDelta.y / Editor->zoom / (float)size;
    }
  } else if (P1Inputs->leftclick == 2) {
    if (Global->pause) {
      glm::vec2 mouse = MouseToScreenpos();
      glm::vec3 temp = ToWorldSpace(mouse);
      Global->Points.push_back(temp);
      Editor->currentlyselectedpoint = Global->Points.size() - 1;
    } else {
      bool check = false;
      for (int i = 0; i < Global->Points.size(); i++) {
        if (ScreenPointMouseDetect(ToScreenSpace(Global->Points[i]))) {
          if (Editor->currentlyselectedpoint == -1 ||
              (Editor->currentlyselectedpoint > -1 &&
               Global->Points[i].z >
                   Global->Points[Editor->currentlyselectedpoint].z)) {
            Editor->currentlyselectedpoint = i;
            Editor->currentlyselectedface = -1;
            check = true;
          }
        }
      }
      if (!check) {
        Editor->currentlyselectedpoint = -1;
        for (int i = 0; i < Global->mapfaces.size(); i++) {
          if (Vec2inTri(
                  MouseToScreenpos(),
                  ToScreenSpace(Global->Points[Global->mapfaces[i].points[0]])
                      .p,
                  ToScreenSpace(Global->Points[Global->mapfaces[i].points[1]])
                      .p,
                  ToScreenSpace(Global->Points[Global->mapfaces[i].points[2]])
                      .p)) {
            check = true;
            Editor->currentlyselectedface = i;
            break;
          }
          if (!check) Editor->currentlyselectedface = -1;
        }
      }
    }
  } else if (P1Inputs->leftclick == 1 && (Editor->currentlyselectedpoint > -1 ||
                                          Editor->currentlyselectedface > -1)) {
    if (Editor->currentlyselectedpoint > -1) {  // point
      glm::vec2 mouse = MouseToScreenpos();
      glm::vec3 temp = ToWorldSpace(mouse);
      temp.x = std::roundf(temp.x);
      temp.y = std::roundf(temp.y);
      Global->Points[Editor->currentlyselectedpoint].x = temp.x;
      Global->Points[Editor->currentlyselectedpoint].y = temp.y;
    } else {  // face
    }
  } else {
    Editor->currentlyselectedpoint = -1;
    Editor->currentlyselectedface = -1;
  }

  Editor->zoom -= P1Inputs->MouseScroll.y;
  if (Editor->zoom < 0.125f) Editor->zoom = 0.125f;
  if (Editor->zoom > 16.f) Editor->zoom = 16.f;

  P1Inputs->MouseDelta.x = 0;
  P1Inputs->MouseDelta.y = 0;
  P1Inputs->MouseScroll.x = 0;
  P1Inputs->MouseScroll.y = 0;
}

void update() {
  if (!Global->isopeningfile) {
    if (P1Inputs->ESC == 2) {
      Global->pause = !Global->pause;
    }
    noncamerastuff();
    movecamera();
    changeUIindex();
    componentsupdatelate();
  }
}