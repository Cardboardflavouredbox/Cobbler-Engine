#include <SDL3/SDL_log.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "camera.h"
#include "deltaTime.h"
#include "extern.h"
#include "map.h"
#include "model.h"
#include "render.h"
#include "rendermath.h"
#include "screen.h"
#include "settings.h"
#include "update.h"

float getdistancething(glm::vec3 P) {
  glm::vec3 p1 = P - LocalPlayer->position;
  float ps = std::sin(LocalPlayer->dir.x * PI / 180.f);
  float pc = std::cos(LocalPlayer->dir.x * PI / 180.f);
  glm::quat q = glm::angleAxis(glm::radians(LocalPlayer->dir.y),
                               glm::vec3(-pc, -ps, 0.0f));

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
  p1 = invisible - LocalPlayer->position;
  p2 = visible - LocalPlayer->position;
  float ps = std::sin(LocalPlayer->dir.x * PI / 180.f);
  float pc = std::cos(LocalPlayer->dir.x * PI / 180.f);
  glm::quat q = glm::angleAxis(glm::radians(LocalPlayer->dir.y),
                               glm::vec3(-pc, -ps, 0.0f));

  p1 = q * p1;
  p2 = q * p2;

  float u = (p1.y * pc - p1.x * ps - 0.25f) /
            (-ps * (p1.x - p2.x) + pc * (p1.y - p2.y));

  glm::vec3 result = (invisible + ((visible - invisible) * u));
  return result;
}

// software rendering stuff
// void rendergame() {
//   std::vector<Mapface> tempmapfacevector = GlobalMapStuff->mapfaces,
//   addlaterfacevector; std::vector<MapPoint> temppointsvector =
//   Global->Points;

//   for (int i = 0; i < tempmapfacevector.size(); i++) {
//     Mapface* tempmapface = &tempmapfacevector[i];
//     float dist[3] = {};
//     std::vector<int> invisiblevector, visiblevector;
//     int invisiblecount = 0;
//     for (int j = 0; j < 3; j++) {
//       dist[j] =
//       getdistancething(temppointsvector[tempmapface->points[j]].pos); if
//       (dist[j] < 0.25f) {
//         invisiblecount++;
//         invisiblevector.push_back(j);
//       } else
//         visiblevector.push_back(j);
//     }
//     switch (invisiblecount) {
//       case 3: {
//         tempmapfacevector.erase(tempmapfacevector.begin() + i);
//         i--;
//         break;
//       }
//       case 2: {
//         for (int j = 0; j < 2; j++) {
//           glm::vec3 newvec3 = CutLinething(
//               temppointsvector[tempmapface->points[invisiblevector[j]]].pos,
//               temppointsvector[tempmapface->points[visiblevector[0]]].pos);
//           temppointsvector.push_back(newvec3);

//           float internal = getinternaldivisionthing(
//               temppointsvector[tempmapface->points[invisiblevector[j]]],
//               newvec3,
//               temppointsvector[tempmapface->points[visiblevector[0]]]);

//           tempmapface->UVs[invisiblevector[j]] =
//               divisiontoVec2(tempmapface->UVs[invisiblevector[j]],
//                              tempmapface->UVs[visiblevector[0]], internal);

//           tempmapface->points[invisiblevector[j]] = temppointsvector.size() -
//           1;
//         }
//         break;
//       }
//       case 1: {
//         Mapface newface;
//         newface.texture = tempmapface->texture;
//         newface.xloop = tempmapface->xloop;
//         newface.yloop = tempmapface->yloop;
//         newface.doublesided = tempmapface->doublesided;
//         newface.points.resize(3);
//         newface.UVs.resize(3);
//         newface.points[visiblevector[0]] =
//             tempmapface->points[visiblevector[0]];
//         newface.UVs[visiblevector[0]] = tempmapface->UVs[visiblevector[0]];
//         glm::vec3 newvec3;
//         newvec3 = CutLinething(
//             temppointsvector[tempmapface->points[invisiblevector[0]]],
//             temppointsvector[tempmapface->points[visiblevector[0]]]);

//         float internal = getinternaldivisionthing(
//             temppointsvector[tempmapface->points[invisiblevector[0]]],
//             newvec3,
//             temppointsvector[tempmapface->points[visiblevector[0]]]);

//         newface.UVs[invisiblevector[0]] =
//             divisiontoVec2(tempmapface->UVs[invisiblevector[0]],
//                            tempmapface->UVs[visiblevector[0]], internal);

//         temppointsvector.push_back(newvec3);

//         newface.points[invisiblevector[0]] = temppointsvector.size() - 1;
//         newvec3 = CutLinething(
//             temppointsvector[tempmapface->points[invisiblevector[0]]],
//             temppointsvector[tempmapface->points[visiblevector[1]]]);

//         internal = getinternaldivisionthing(
//             temppointsvector[tempmapface->points[invisiblevector[0]]],
//             newvec3,
//             temppointsvector[tempmapface->points[visiblevector[1]]]);

//         newface.UVs[visiblevector[1]] =
//             divisiontoVec2(tempmapface->UVs[invisiblevector[0]],
//                            tempmapface->UVs[visiblevector[1]], internal);

//         temppointsvector.push_back(newvec3);
//         tempmapface->points[invisiblevector[0]] = temppointsvector.size() -
//         1; tempmapface->UVs[invisiblevector[0]] =
//         newface.UVs[visiblevector[1]]; newface.points[visiblevector[1]] =
//         temppointsvector.size() - 1; newface.shade = tempmapface->shade;
//         addlaterfacevector.push_back(newface);
//         break;
//       }
//     }
//   }

//   tempmapfacevector.insert(tempmapfacevector.end(),
//   addlaterfacevector.begin(),
//                            addlaterfacevector.end());

//   for (int k = 0; k < tempmapfacevector.size(); k++) {
//     glm::vec3 temp[3] = {temppointsvector[tempmapfacevector[k].points[0]],
//                          temppointsvector[tempmapfacevector[k].points[1]],
//                          temppointsvector[tempmapfacevector[k].points[2]]};
//     glm::vec2 temp2[3] = {tempmapfacevector[k].UVs[0],
//                           tempmapfacevector[k].UVs[1],
//                           tempmapfacevector[k].UVs[2]};
//     DrawTri(tempmapfacevector[k].texture, temp, temp2,
//             tempmapfacevector[k].xloop, tempmapfacevector[k].yloop);
//   }
// }

void render3DUI() {
  for (int i = 0; i < UIGlobalStuff->UIlist.size(); i++) {
    int len = UIGlobalStuff->UImap3D[UIGlobalStuff->UIlist[i]].size();
    for (int j = 0; j < len; j++) {
      Modeltransform* model =
          UIGlobalStuff->UImap3D[UIGlobalStuff->UIlist[i]][j]->modeltrans;
      ModelGroupClass* modelgroup = &ModelGroupMap[model->name];

      // SDL_Log("%f %f %f", model->position.x, model->position.y,
      //         model->position.z);

      renderModelGroup(model, modelgroup, true, deltaTime);
    }
  }
}

void render2DUI() {
  for (int i = 0; i < UIGlobalStuff->UIlist.size(); i++) {
    int len = UIGlobalStuff->UImap[UIGlobalStuff->UIlist[i]].size();
    for (int j = 0; j < len; j++) {
      if (Settings->graphicsmode == 1) glDisable(GL_TEXTURE_2D);
      UIGlobalStuff->UImap[UIGlobalStuff->UIlist[i]].at(j)->render();
    }
  }
}

void renderEntity() {
  for (const auto& i : Global->Entities) {
    if (i.second->Modelthing != nullptr) i.second->rendermodelgroup();
  }
  for (const auto& i : GlobalNetworkStuff->PlayerEntity) {
    if (i.second->Modelthing != nullptr) i.second->rendermodelgroup();
  }
}

void renderProps() {
  for (int i = 0; i < Global->Models.size(); i++) {
    Modeltransform* model = &Global->Models[i];
    ModelGroupClass* modelgroup = &ModelGroupMap[model->name];

    renderModelGroup(model, modelgroup, false, deltaTime);
  }
}

void renderParticles() {
  float ps = std::sin(LocalPlayer->dir.x * PI / 180.f);
  float pc = std::cos(LocalPlayer->dir.x * PI / 180.f);
  float what = std::cos(LocalPlayer->dir.y * PI / 180.f);

  glm::vec3 dirthing = glm::normalize(glm::vec3{
      -ps * what, pc * what, std::sin(LocalPlayer->dir.y * PI / 180.f)});

  glm::quat quatthing =
      glm::quatLookAt(-dirthing, glm::vec3(0, 0, 1)) *
      glm::quatLookAt(glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));

  for (const auto& [key, i] : Global->Particles) {
    if (i->Texture == "") {
      glDisable(GL_TEXTURE_2D);
    } else {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,
                    RendererGlobal->GLstuff->textures[i->Texture]);
    }

    glm::vec3 quad[4];

    quad[0].x = i->rect[0].x;
    quad[0].y = 0;
    quad[0].z = i->rect[0].y;

    quad[1].x = i->rect[1].x;
    quad[1].y = 0;
    quad[1].z = i->rect[0].y;

    quad[2].x = i->rect[1].x;
    quad[2].y = 0;
    quad[2].z = i->rect[1].y;

    quad[3].x = i->rect[0].x;
    quad[3].y = 0;
    quad[3].z = i->rect[1].y;

    for (int j = 0; j < 4; j++) {
      quad[j] = quatthing * quad[j] + i->position;
    }

    glBegin(GL_QUADS);
    glColor4f(i->color[0], i->color[1], i->color[2], i->color[3]);
    glTexCoord2f(i->uv[0].x, i->uv[0].y);
    glVertex3f(quad[0].x, quad[0].y, quad[0].z);
    glTexCoord2f(i->uv[0].x + i->uv[1].x, i->uv[0].y);
    glVertex3f(quad[1].x, quad[1].y, quad[1].z);
    glTexCoord2f(i->uv[0].x + i->uv[1].x, i->uv[0].y + i->uv[1].y);
    glVertex3f(quad[2].x, quad[2].y, quad[2].z);
    glTexCoord2f(i->uv[0].x, i->uv[0].y + i->uv[1].y);
    glVertex3f(quad[3].x, quad[3].y, quad[3].z);
    glEnd();
  }
}

void renderbackground() {
  int x = Settings->resolutionx, y = Settings->resolutiony;
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      if (RendererGlobal->SRstuff
              ->pixelsdepth[i + j * RendererGlobal->SRstuff->pitch] == 65535) {
        Uint8 color = static_cast<Uint8*>(
            RendererGlobal->SRstuff->textures[GlobalMapStuff->skybox]
                ->pixels)[(int(i * 320.f / x) +
                           int((1 - ((int(LocalPlayer->dir.x) % 180) / 180.f)) *
                               640.f)) %
                              640 +
                          (int((1 - (LocalPlayer->dir.y) / 90.f) * 200.f) +
                           int(j * 200.f / y)) *
                              640];
        RendererGlobal->SRstuff
            ->pixels[i + j * RendererGlobal->SRstuff->pitch] = color;
      }
    }
  }
}

void softwarerender() {
  SDL_Log("Software renderer is not supported anymore.");
  Global->IsRunning = false;
  // SDL_LockSurface(RendererGlobal->SRstuff->render_target);

  // RendererGlobal->SRstuff->pixels =
  //     static_cast<unsigned
  //     char*>(RendererGlobal->SRstuff->render_target->pixels);
  // RendererGlobal->SRstuff->pitch =
  // RendererGlobal->SRstuff->render_target->pitch;

  // for (int i = 0; i < Settings->resolutionx; i++) {
  //   for (int j = 0; j < Settings->resolutiony; j++) {
  //     RendererGlobal->SRstuff->pixelsdepth[i + j *
  //     RendererGlobal->SRstuff->pitch] = 65535;
  //   }
  // }
  // render2DUI();
  // render3DUI();
  // rendergame();
  // renderProps();
  // renderEntity();

  // renderbackground();

  // SDL_UnlockSurface(RendererGlobal->SRstuff->render_target);

  // // Screen size and position stuff
  // int w = Global->windowx, h = Global->windowy, rtw = Settings->resolutionx,
  //     rth = Settings->resolutiony;
  // int size = w / rtw;
  // if (size > h / rth) size = h / rth;

  // rtw *= size;
  // rth *= size;

  // w /= 2;
  // h /= 2;
  // w -= rtw / 2;
  // h -= rth / 2;

  // SDL_FRect temprect;
  // temprect.w = rtw;
  // temprect.h = rth;
  // temprect.x = w;
  // temprect.y = h;
  // SDL_Texture* temptexture = SDL_CreateTextureFromSurface(
  //     RendererGlobal->SRstuff->renderer,
  //     RendererGlobal->SRstuff->render_target);
  // SDL_SetTextureScaleMode(temptexture, SDL_SCALEMODE_PIXELART);
  // SDL_RenderTexture(RendererGlobal->SRstuff->renderer, temptexture, NULL,
  // &temprect); SDL_RenderPresent(RendererGlobal->SRstuff->renderer);
}

void openglrender() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor4f(1, 1, 1, 1);

  glm::mat4 modelMatrix = Global->perspectivematrix;

  glm::mat4 view = glm::lookAt(Camera->pos, Camera->lookat, glm::vec3(0, 0, 1));

  modelMatrix = modelMatrix * view;

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(glm::value_ptr(modelMatrix));

  // OpenGL rendering goes here
  glCallList(RendererGlobal->GLstuff->MapGLlist);

  renderProps();
  renderEntity();

  renderParticles();

  glLoadIdentity();

  glClear(GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  modelMatrix = Global->perspectivematrix;

  view = glm::lookAt(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));

  modelMatrix = modelMatrix * view;

  glLoadMatrixf(glm::value_ptr(modelMatrix));
  render3DUI();

  glMatrixMode(GL_PROJECTION);
  glOrtho(0, Settings->resolutionx, 0, Settings->resolutiony, -1, 1);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);

  if (Global->pause) {
    glBegin(GL_TRIANGLE_FAN);

    glColor4f(0, 0, 0, 0.5f);

    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);

    glEnd();
  }

  render2DUI();

  glFlush();

  SDL_GL_SwapWindow(RendererGlobal->window);
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