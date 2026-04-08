#include <SDL3/SDL.h>

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

// https://gamedev.stackexchange.com/questions/172308/c-mouse-picking-for-ray-to-plane-intersection
// check this later
glm::vec3 Vec2Ray() {
  float ps = std::sin(Camera->dir.x * PI / 180.0);
  float pc = std::cos(Camera->dir.x * PI / 180.0);
  float whats = std::sin(Camera->dir.y * PI / 180.0);
  float whatc = std::cos(Camera->dir.y * PI / 180.0);
  glm::vec2 mouse = MouseToScreenpos();
  mouse.x = (mouse.x - (Settings->resolutionx / 2)) /
            (float)Settings->resolutionx * 2;
  mouse.y = (mouse.y - (Settings->resolutiony / 2)) /
            (float)Settings->resolutiony * 2;
  mouse.y *= -1;

  SDL_Log("%f %f", mouse.x, mouse.y);

  glm::vec3 dirtemp;
  dirtemp.x = -ps * whatc;
  dirtemp.y = pc * whatc;
  dirtemp.z = whats;
  glm::mat4 proj = glm::perspective(
      (float)Settings->fov,
      (float)Settings->resolutionx / (float)Settings->resolutiony, 0.25f, 64.f);
  glm::mat4 view = glm::lookAt(Camera->position, Camera->position + dirtemp,
                               glm::vec3({0, 0, 1}));

  glm::mat4 invVP = glm::inverse(proj * view);
  glm::vec4 screenPos = glm::vec4(mouse.x, mouse.y, 1.0f, 1.0f);
  glm::vec4 worldPos = invVP * screenPos;

  glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

  return dir;
}

void noncamerastuff() {
  if (Global->rendermode == 1) {
    if (P1Inputs->leftclick > 0) {
      if (Global->editordraggingaxis == -1) {
        glm::vec3 CurrentPoint = Global->Points[Global->editorselectedPoint],
                  temp[3] = {glm::vec3({4, 0, 0}), glm::vec3({0, 4, 0}),
                             glm::vec3({0, 0, 4})};
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
        for (int i = 0; i < 3; i++) {
          if (ScreenPointMouseDetect(ToScreenSpace((CurrentPoint + temp[i])))) {
            Global->editordraggingaxis = i;
            break;
          }
        }
        if (P1Inputs->leftclick > 1) {
          for (int i = 0; i < Global->Points.size(); i++) {
            if (ScreenPointMouseDetect(ToScreenSpace(Global->Points[i]))) {
              Global->editorselectedPoint = i;
            }
          }
        }
      }
    }
  } else if (Global->rendermode == 0) {
    if (P1Inputs->leftclick > 1) {
      float ps = std::sin(Camera->dir.x * PI / 180.0);
      float pc = std::cos(Camera->dir.x * PI / 180.0);
      float whats = std::sin(Camera->dir.y * PI / 180.0);
      float whatc = std::cos(Camera->dir.y * PI / 180.0);
      glm::vec3 ray[2] = {(Camera->position),
                          (Camera->position + Vec2Ray() * 32.f)};
      SDL_Log("%f %f %f", ray[1].x, ray[1].y, ray[1].z);
      float dist = INFINITY;
      glm::vec3 output;
      for (int i = 0; i < Global->mapfaces.size(); i++) {
        if (RayTriCheck(Global->Points[Global->mapfaces[i].points[0]],
                        Global->Points[Global->mapfaces[i].points[1]],
                        Global->Points[Global->mapfaces[i].points[2]], ray[0],
                        ray[1], output) &&
            glm::distance(output, Camera->position) < dist) {
          dist = glm::distance(output, Camera->position);
          Global->editorselectedFace = i;
        }
      }
    }
  }
  if (Global->editordraggingaxis > -1) {
    if (P1Inputs->leftclick == 0)
      Global->editordraggingaxis = -1;
    else {
      float ps = std::sin(Camera->dir.x * PI / 180.f);
      float pc = std::cos(Camera->dir.x * PI / 180.f);
      switch (Global->editordraggingaxis) {
        case 0: {
          Global->Points[Global->editorselectedPoint].x +=
              (P1Inputs->MouseDelta.x * pc + P1Inputs->MouseDelta.y * ps) /
                  16.f -
              std::fmod(
                  (P1Inputs->MouseDelta.x * pc + P1Inputs->MouseDelta.y * ps) /
                      16.f,
                  0.125f);
          break;
        }
        case 1: {
          Global->Points[Global->editorselectedPoint].y +=
              (P1Inputs->MouseDelta.x * ps - P1Inputs->MouseDelta.y * pc) /
                  16.f -
              std::fmod(
                  (P1Inputs->MouseDelta.x * ps - P1Inputs->MouseDelta.y * pc) /
                      16.f,
                  0.125f);
          break;
        }
        case 2: {
          Global->Points[Global->editorselectedPoint].z +=
              (-P1Inputs->MouseDelta.y) / 16.f -
              std::fmod((-P1Inputs->MouseDelta.y) / 16.f, 0.125f);
          break;
        }
      }
      // Global->Points[Global->editorselectedPoint].x -=
      //     std::fmodf(Global->Points[Global->editorselectedPoint].x, 0.25f);
      // Global->Points[Global->editorselectedPoint].y -=
      //     std::fmodf(Global->Points[Global->editorselectedPoint].y, 0.25f);
      // Global->Points[Global->editorselectedPoint].z -=
      //     std::fmodf(Global->Points[Global->editorselectedPoint].z, 0.25f);
    }
  }
  if (P1Inputs->F == 2) {
    glm::vec3 temp = Global->Points[Global->editorselectedPoint];

    Camera->position =
        (temp + (glm::normalize((Camera->position - temp))) * 4.f);
  }
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