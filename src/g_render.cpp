#include <SDL3/SDL_log.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "extern.h"
#include "map.h"
#include "model.h"
#include "render.h"
#include "rendermath.h"
#include "screen.h"
#include "update.h"

float getdistancething(glm::vec3 P) {
  glm::vec3 p1 = P - Camera->position;
  float ps = std::sin(Camera->dir.x * PI / 180.f);
  float pc = std::cos(Camera->dir.x * PI / 180.f);
  glm::quat q =
      glm::angleAxis(glm::radians(Camera->dir.y), glm::vec3(-pc, -ps, 0.0f));

  p1 = q * p1;
  return p1.y * pc - p1.x * ps;
}

float getinternaldivisionthing(glm::vec3 p1, glm::vec3 d, glm::vec3 p2) {
  float dist1 = glm::distance(p1, d), dist2 = glm::distance(d, p2);
  return dist1 / (dist1 + dist2);
}

glm::vec2 divisiontoVec2(glm::vec2 p1, glm::vec2 p2, float t) {
  return glm::vec2({p2.x * t + p1.x * (1 - t), p2.y * t + p1.y * (1 - t)});
}

glm::vec3 CutLinething(glm::vec3 invisible, glm::vec3 visible) {
  glm::vec3 p1, p2;
  p1 = invisible - Camera->position;
  p2 = visible - Camera->position;
  float ps = std::sin(Camera->dir.x * PI / 180.f);
  float pc = std::cos(Camera->dir.x * PI / 180.f);
  glm::quat q =
      glm::angleAxis(glm::radians(Camera->dir.y), glm::vec3(-pc, -ps, 0.0f));

  p1 = q * p1;
  p2 = q * p2;

  float u = (p1.y * pc - p1.x * ps - 0.25f) /
            (-ps * (p1.x - p2.x) + pc * (p1.y - p2.y));

  glm::vec3 result = (invisible + ((visible - invisible) * u));
  return result;
}

void rendergame() {
  std::vector<Mapface> tempmapfacevector = Global->mapfaces, addlaterfacevector;
  std::vector<glm::vec3> temppointsvector = Global->Points;

  for (int i = 0; i < tempmapfacevector.size(); i++) {
    Mapface* tempmapface = &tempmapfacevector[i];
    float dist[3] = {};
    std::vector<int> invisiblevector, visiblevector;
    int invisiblecount = 0;
    for (int j = 0; j < 3; j++) {
      dist[j] = getdistancething(temppointsvector[tempmapface->points[j]]);
      if (dist[j] < 0.25f) {
        invisiblecount++;
        invisiblevector.push_back(j);
      } else
        visiblevector.push_back(j);
    }
    switch (invisiblecount) {
      case 3: {
        tempmapfacevector.erase(tempmapfacevector.begin() + i);
        i--;
        break;
      }
      case 2: {
        for (int j = 0; j < 2; j++) {
          glm::vec3 newvec3 = CutLinething(
              temppointsvector[tempmapface->points[invisiblevector[j]]],
              temppointsvector[tempmapface->points[visiblevector[0]]]);
          temppointsvector.push_back(newvec3);

          float internal = getinternaldivisionthing(
              temppointsvector[tempmapface->points[invisiblevector[j]]],
              newvec3, temppointsvector[tempmapface->points[visiblevector[0]]]);

          tempmapface->UVs[invisiblevector[j]] =
              divisiontoVec2(tempmapface->UVs[invisiblevector[j]],
                             tempmapface->UVs[visiblevector[0]], internal);

          tempmapface->points[invisiblevector[j]] = temppointsvector.size() - 1;
        }
        break;
      }
      case 1: {
        Mapface newface;
        newface.texture = tempmapface->texture;
        newface.xloop = tempmapface->xloop;
        newface.yloop = tempmapface->yloop;
        newface.doublesided = tempmapface->doublesided;
        newface.points.resize(3);
        newface.UVs.resize(3);
        newface.points[visiblevector[0]] =
            tempmapface->points[visiblevector[0]];
        newface.UVs[visiblevector[0]] = tempmapface->UVs[visiblevector[0]];
        glm::vec3 newvec3;
        newvec3 = CutLinething(
            temppointsvector[tempmapface->points[invisiblevector[0]]],
            temppointsvector[tempmapface->points[visiblevector[0]]]);

        float internal = getinternaldivisionthing(
            temppointsvector[tempmapface->points[invisiblevector[0]]], newvec3,
            temppointsvector[tempmapface->points[visiblevector[0]]]);

        newface.UVs[invisiblevector[0]] =
            divisiontoVec2(tempmapface->UVs[invisiblevector[0]],
                           tempmapface->UVs[visiblevector[0]], internal);

        temppointsvector.push_back(newvec3);

        newface.points[invisiblevector[0]] = temppointsvector.size() - 1;
        newvec3 = CutLinething(
            temppointsvector[tempmapface->points[invisiblevector[0]]],
            temppointsvector[tempmapface->points[visiblevector[1]]]);

        internal = getinternaldivisionthing(
            temppointsvector[tempmapface->points[invisiblevector[0]]], newvec3,
            temppointsvector[tempmapface->points[visiblevector[1]]]);

        newface.UVs[visiblevector[1]] =
            divisiontoVec2(tempmapface->UVs[invisiblevector[0]],
                           tempmapface->UVs[visiblevector[1]], internal);

        temppointsvector.push_back(newvec3);
        tempmapface->points[invisiblevector[0]] = temppointsvector.size() - 1;
        tempmapface->UVs[invisiblevector[0]] = newface.UVs[visiblevector[1]];
        newface.points[visiblevector[1]] = temppointsvector.size() - 1;
        newface.shade = tempmapface->shade;
        addlaterfacevector.push_back(newface);
        break;
      }
    }
  }

  tempmapfacevector.insert(tempmapfacevector.end(), addlaterfacevector.begin(),
                           addlaterfacevector.end());

  for (int k = 0; k < tempmapfacevector.size(); k++) {
    glm::vec3 temp[3] = {temppointsvector[tempmapfacevector[k].points[0]],
                         temppointsvector[tempmapfacevector[k].points[1]],
                         temppointsvector[tempmapfacevector[k].points[2]]};
    glm::vec2 temp2[3] = {tempmapfacevector[k].UVs[0],
                          tempmapfacevector[k].UVs[1],
                          tempmapfacevector[k].UVs[2]};
    DrawTri(tempmapfacevector[k].texture, temp, temp2,
            tempmapfacevector[k].xloop, tempmapfacevector[k].yloop);
  }
}

void renderUI() {
  for (int i = 0; i < Global->UIlist.size(); i++) {
    int len = Global->UImap[Global->UIlist[i]].size();
    for (int j = 0; j < len; j++) {
      if (Settings->graphicsmode == 1) glDisable(GL_TEXTURE_2D);
      Global->UImap[Global->UIlist[i]].at(j)->render();
    }
  }
}

void renderEntity() {
  for (int i = 0; i < Global->Entities.size(); i++) {
    if (Global->Entities[i]->billboardthing != nullptr)
      Global->Entities[i]->renderbillboard();
    else if (Global->Entities[i]->Modelthing != nullptr)
      Global->Entities[i]->rendermodelgroup();
  }
}

void renderProps() {
  for (int i = 0; i < Global->Models.size(); i++) {
    ModelGroupClass* modelgroup = &ModelGroupMap[Global->Models[i].name];

    Global->Models[i].frame += Global->deltaTime * 5 / 2;
    while (Global->Models[i].frame >= (float)modelgroup->animend)
      Global->Models[i].frame +=
          ((float)modelgroup->animstart - (float)modelgroup->animend);
    if (Global->Models[i].frame < (float)modelgroup->animstart)
      Global->Models[i].frame = (float)modelgroup->animstart;

    renderModelGroup(Global->Models[i], modelgroup);
  }
}

void renderbackground() {
  int x = Settings->resolutionx, y = Settings->resolutiony;
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      if (Global->SRstuff->pixelsdepth[i + j * Global->SRstuff->pitch] ==
          65535) {
        Uint8 color = static_cast<Uint8*>(
            Global->SRstuff->textures[Global->skybox]->pixels)
            [(int(i * 320.f / x) +
              int((1 - ((int(Camera->dir.x) % 180) / 180.f)) * 640.f)) %
                 640 +
             (int((1 - (Camera->dir.y) / 90.f) * 200.f) + int(j * 200.f / y)) *
                 640];
        Global->SRstuff->pixels[i + j * Global->SRstuff->pitch] = color;
      }
    }
  }
}

void softwarerender() {
  SDL_LockSurface(Global->SRstuff->render_target);

  Global->SRstuff->pixels =
      static_cast<unsigned char*>(Global->SRstuff->render_target->pixels);
  Global->SRstuff->pitch = Global->SRstuff->render_target->pitch;

  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      Global->SRstuff->pixelsdepth[i + j * Global->SRstuff->pitch] = 65535;
    }
  }
  renderUI();
  rendergame();
  renderProps();
  renderEntity();

  renderbackground();

  SDL_UnlockSurface(Global->SRstuff->render_target);

  // Screen size and position stuff
  int w = Global->windowx, h = Global->windowy, rtw = Settings->resolutionx,
      rth = Settings->resolutiony;
  int size = w / rtw;
  if (size > h / rth) size = h / rth;

  rtw *= size;
  rth *= size;

  w /= 2;
  h /= 2;
  w -= rtw / 2;
  h -= rth / 2;

  SDL_FRect temprect;
  temprect.w = rtw;
  temprect.h = rth;
  temprect.x = w;
  temprect.y = h;
  SDL_Texture* temptexture = SDL_CreateTextureFromSurface(
      Global->SRstuff->renderer, Global->SRstuff->render_target);
  SDL_SetTextureScaleMode(temptexture, SDL_SCALEMODE_PIXELART);
  SDL_RenderTexture(Global->SRstuff->renderer, temptexture, NULL, &temprect);
  SDL_RenderPresent(Global->SRstuff->renderer);
}

void openglrender() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor4f(1, 1, 1, 1);

  glm::vec3 lookdir;
  lookdir.x = std::cos(glm::radians(Camera->dir.x + 90.f)) *
              std::cos(glm::radians(Camera->dir.y));
  lookdir.z = std::sin(glm::radians(Camera->dir.y));
  lookdir.y = std::sin(glm::radians(Camera->dir.x + 90.f)) *
              std::cos(glm::radians(Camera->dir.y));
  glm::mat4 modelMatrix = Global->perspectivematrix;

  glm::mat4 view =
      glm::lookAt(glm::vec3(0), lookdir * 16.f, glm::vec3(0, 0, 1));

  modelMatrix = modelMatrix * view;

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(glm::value_ptr(modelMatrix));

  // OpenGL rendering goes here
  for (int i = 0; i < Global->mapfaces.size(); i++) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,
                  Global->GLstuff->textures[Global->mapfaces[i].texture]);
    glBegin(GL_TRIANGLES);
    for (int j = 0; j < 3; j++) {
      glm::vec3 pos = Global->Points[Global->mapfaces[i].points[j]];
      glm::vec2 uvw = Global->mapfaces[i].UVs[j];
      pos -= Camera->position;
      glTexCoord2f(uvw.x * Global->mapfaces[i].xloop,
                   uvw.y * Global->mapfaces[i].yloop);
      glVertex3f(pos.x, pos.y, pos.z);
    }
    glEnd();
  }
  renderProps();
  renderEntity();

  glMatrixMode(GL_PROJECTION);
  glOrtho(0, Settings->resolutionx, 0, Settings->resolutiony, -1, 1);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);

  if (Global->pause || Global->isopeningfile) {
    glBegin(GL_TRIANGLE_FAN);

    glColor4f(0, 0, 0, 0.5f);

    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);

    glEnd();
  }

  renderUI();

  glFlush();

  SDL_GL_SwapWindow(Global->window);
}

void render() {
  switch (Settings->graphicsmode) {
    case 1:
      openglrender();
      break;
    default:
      softwarerender();
  }
}