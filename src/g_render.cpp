#include <SDL3/SDL_log.h>

#include <deque>
#include <glm/glm.hpp>
#include <string>

#include "extern.h"
#include "map.h"
#include "rendermath.h"
#include "screen.h"
#include "update.h"

float getdistancething(glm::vec3 P) {
  glm::vec3 p1 = P - Camera->position;
  float ps = std::sin(Camera->dir.x * PI / 180.f);
  float pc = std::cos(Camera->dir.x * PI / 180.f);
  float what = std::sin(Camera->dir.y * PI / 180.f);
  return p1.y * pc - p1.x * ps + p1.z * what;
}

ScreenPoint ToScreenSpace(glm::vec3 P) {
  glm::vec3 p1 = (P - Camera->position);

  float ps = std::sin(Camera->dir.x * PI / 180.f);
  float pc = std::cos(Camera->dir.x * PI / 180.f);
  float whats = std::sin(Camera->dir.y * PI / 180.f);

  float tx = p1.x * pc + p1.y * ps;
  float ty = p1.y * pc - p1.x * ps + p1.z * whats;
  float tz = 180 * whats;

  ScreenPoint screenpos;
  if (ty <= 0.25f) {
    screenpos.isbehindcamera = true;
    ty = 0.25f;
  }
  screenpos.p.x = (tx * Settings->fov / ty) + (Settings->resolutionx / 2);
  screenpos.p.y =
      (-p1.z * Settings->fov / ty) + (Settings->resolutiony / 2) + tz;
  screenpos.dist = 1.f / ty;
  return screenpos;
}

void DrawLine(unsigned char color, glm::vec3 rawvectors[]) {
  ScreenPoint vectors[2] = {ToScreenSpace(rawvectors[0]),
                            ToScreenSpace(rawvectors[1])};
  if (!vectors[0].isbehindcamera || !vectors[1].isbehindcamera) {
    int x = vectors[0].p.x, x2 = vectors[1].p.x;

    int y = vectors[0].p.y, y2 = vectors[1].p.y;

    if (std::abs(y2 - y) < std::abs(x2 - x)) {
      if (x > x2) {
        x = vectors[1].p.x;
        x2 = vectors[0].p.x;
        y = vectors[1].p.y;
        y2 = vectors[0].p.y;
      }
      for (int i = x; i <= x2; i++) {
        int tempy = y + ((i - x) * (y2 - y) / (x2 - x));
        if (i >= 0 && tempy >= 0 && i < Settings->resolutionx &&
            tempy < Settings->resolutiony) {
          if (Global->pixelsdepth[i + tempy * Global->pitch] >= 8) {
            Global->pixelsdepth[i + tempy * Global->pitch] = 8;
            Global->pixels[i + tempy * Global->pitch] = color;
          }
        }
      }
    } else {
      if (y > y2) {
        x = vectors[1].p.x;
        x2 = vectors[0].p.x;
        y = vectors[1].p.y;
        y2 = vectors[0].p.y;
      }
      for (int i = y; i <= y2; i++) {
        int tempx = x + ((i - y) * (x2 - x) / (y2 - y));
        if (tempx >= 0 && i >= 0 && tempx < Settings->resolutionx &&
            i < Settings->resolutiony) {
          if (Global->pixelsdepth[tempx + i * Global->pitch] >= 8) {
            Global->pixelsdepth[tempx + i * Global->pitch] = 8;
            Global->pixels[tempx + i * Global->pitch] = color;
          }
        }
      }
    }
  }
}

void DrawCircle(unsigned char color, glm::vec3 rawpoint, int radius) {
  ScreenPoint point = ToScreenSpace(rawpoint);
  if (!point.isbehindcamera)
    for (int i = point.p.x - radius; i <= point.p.x + radius; i++) {
      for (int j = point.p.y - radius; j < point.p.y + radius; j++) {
        if (i > -1 && i < Settings->resolutionx && j > -1 &&
            j < Settings->resolutiony) {
          if (Global->pixelsdepth[i + j * Global->pitch] >= 7) {
            Global->pixelsdepth[i + j * Global->pitch] = 7;
            Global->pixels[i + j * Global->pitch] = color;
          }
        }
      }
    }
}

void DrawTri(int texture, glm::vec3 rawvectors[], glm::vec2 UVs[], int xloop,
             int yloop, std::array<unsigned char, 4> shade) {
  ScreenPoint vectors[3] = {ToScreenSpace(rawvectors[0]),
                            ToScreenSpace(rawvectors[1]),
                            ToScreenSpace(rawvectors[2])};
  if (!vectors[0].isbehindcamera || !vectors[1].isbehindcamera ||
      !vectors[2].isbehindcamera) {
    int x = vectors[0].p.x, x2 = vectors[0].p.x, y = vectors[0].p.y,
        y2 = vectors[0].p.y;
    for (int i = 1; i < 3; i++) {
      if (vectors[i].p.x < x) x = vectors[i].p.x;
      if (vectors[i].p.x > x2) x2 = vectors[i].p.x;
      if (vectors[i].p.y < y) y = vectors[i].p.y;
      if (vectors[i].p.y > y2) y2 = vectors[i].p.y;
    }
    if (x < 0) x = 0;
    if (x >= Settings->resolutionx - 1) x = Settings->resolutionx - 1;
    if (x2 < 0) x2 = 0;
    if (x2 >= Settings->resolutionx - 1) x2 = Settings->resolutionx - 1;
    if (y < 0) y = 0;
    if (y >= Settings->resolutiony - 1) y = Settings->resolutiony - 1;
    if (y2 < 0) y2 = 0;
    if (y2 >= Settings->resolutiony - 1) y2 = Settings->resolutiony - 1;
    for (int i = x; i <= x2; i++) {
      for (int j = y; j <= y2; j++) {
        glm::vec2 temp;
        temp.x = i;
        temp.y = j;
        if (temp.x >= 0 && temp.y >= 0 && temp.x < Settings->resolutionx &&
            temp.y < Settings->resolutiony) {
          if (Vec2inTri(temp, vectors[0].p, vectors[1].p, vectors[2].p)) {
            glm::vec3 uvw =
                GetUV(temp, vectors[0].p, vectors[1].p, vectors[2].p);
            glm::vec2 uvresult = ((((UVs[0] * uvw.x) * vectors[0].dist) +
                                   ((UVs[1] * uvw.y) * vectors[1].dist)) +
                                  ((UVs[2] * uvw.z) * vectors[2].dist));
            uvresult = (uvresult * (1 / (uvw.x * vectors[0].dist +
                                         uvw.y * vectors[1].dist +
                                         uvw.z * vectors[2].dist)));

            int uvxthing = (int(128 * (uvresult.x)) * xloop) % 128;
            int uvything = (int(128 * (uvresult.y)) * yloop) % 128;
            Uint32 color = static_cast<Uint32*>(
                Global->textures[texture]->pixels)[uvxthing + uvything * 128];

            int r = (color >> 0) & 0xFF;
            int g = (color >> 8) & 0xFF;
            int b = (color >> 16) & 0xFF;
            int a = (color >> 24) & 0xFF;

            glm::vec3 tempvec3;
            tempvec3.x = rawvectors[0].x * uvw.x + rawvectors[1].x * uvw.y +
                         rawvectors[2].x * uvw.z;
            tempvec3.y = rawvectors[0].y * uvw.x + rawvectors[1].y * uvw.y +
                         rawvectors[2].y * uvw.z;
            tempvec3.z = rawvectors[0].z * uvw.x + rawvectors[1].z * uvw.y +
                         rawvectors[2].z * uvw.z;
            tempvec3.x -= Camera->position.x;
            tempvec3.y -= Camera->position.y;
            tempvec3.z -= Camera->position.z;
            float dist =
                std::sqrt(tempvec3.x * tempvec3.x + tempvec3.y * tempvec3.y +
                          tempvec3.z * tempvec3.z);
            if (Global->pixelsdepth[i + j * Global->pitch] > dist * 3 ||
                Global->pixelstransparency[i + j * Global->pitch] < 255) {
              r -= dist * 3;
              g -= dist * 3;
              b -= dist * 3;
              r = r * (int)shade[0] / 255;
              g = g * (int)shade[1] / 255;
              b = b * (int)shade[2] / 255;
              // r = uvresult.x * 255;
              // g = uvresult.y * 255;
              // b = 0;
              if (Global->pixelstransparency[i + j * Global->pitch] < 255) {
                int transparency =
                    Global->pixelstransparency[i + j * Global->pitch];
                SDL_Color tempcolor =
                    Global->palette
                        ->colors[Global->pixels[i + j * Global->pitch]];
                r = r * (255 - transparency) / 255 +
                    tempcolor.r * transparency / 255;
                g = g * (255 - transparency) / 255 +
                    tempcolor.g * transparency / 255;
                b = b * (255 - transparency) / 255 +
                    tempcolor.b * transparency / 255;
              }
              if (r < 0) r = 0;
              if (g < 0) g = 0;
              if (b < 0) b = 0;
              Global->pixels[i + j * Global->pitch] =
                  SDL_MapSurfaceRGB(Global->render_target, r, g, b);
              if (dist < 0) dist = 0;
              if (Global->pixelstransparency[i + j * Global->pitch] == 255)
                Global->pixelsdepth[i + j * Global->pitch] =
                    (unsigned char)dist * 4;
              Global->pixelstransparency[i + j * Global->pitch] = a;
            }
          }
        }
      }
    }
  }
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
  p1.x = invisible.x - Camera->position.x;
  p1.y = invisible.y - Camera->position.y;
  p1.z = invisible.z - Camera->position.z;
  p2.x = visible.x - Camera->position.x;
  p2.y = visible.y - Camera->position.y;
  p2.z = visible.z - Camera->position.z;
  float ps = std::sin(Camera->dir.x * PI / 180.f);
  float pc = std::cos(Camera->dir.x * PI / 180.f);
  float what = std::sin(Camera->dir.y * PI / 180.f);

  float u = (p1.y * pc - p1.x * ps + p1.z * what - 0.25f) /
            (-ps * (p1.x - p2.x) + pc * (p1.y - p2.y) + what * (p1.z - p2.z));

  glm::vec3 result = (invisible + ((visible - invisible) * u));
  return result;
}

void rendergame() {
  std::deque<Mapface> tempmapfacedeque = Global->mapfaces, addlaterfacedeque;
  std::deque<glm::vec3> temppointsdeque = Global->Points;

  for (int i = 0; i < tempmapfacedeque.size(); i++) {
    Mapface* tempmapface = &tempmapfacedeque[i];
    float dist[3] = {};
    std::deque<int> invisibledeque, visibledeque;
    int invisiblecount = 0;
    for (int j = 0; j < 3; j++) {
      dist[j] = getdistancething(temppointsdeque[tempmapface->points[j]]);
      if (dist[j] < 0.25f) {
        invisiblecount++;
        invisibledeque.push_back(j);
      } else
        visibledeque.push_back(j);
    }
    switch (invisiblecount) {
      case 3: {
        tempmapfacedeque.erase(tempmapfacedeque.begin() + i);
        i--;
        break;
      }
      case 2: {
        for (int j = 0; j < 2; j++) {
          glm::vec3 newvec3 = CutLinething(
              temppointsdeque[tempmapface->points[invisibledeque[j]]],
              temppointsdeque[tempmapface->points[visibledeque[0]]]);
          temppointsdeque.push_back(newvec3);

          float internal = getinternaldivisionthing(
              temppointsdeque[tempmapface->points[invisibledeque[j]]], newvec3,
              temppointsdeque[tempmapface->points[visibledeque[0]]]);

          tempmapface->UVs[invisibledeque[j]] =
              divisiontoVec2(tempmapface->UVs[invisibledeque[j]],
                             tempmapface->UVs[visibledeque[0]], internal);

          tempmapface->points[invisibledeque[j]] = temppointsdeque.size() - 1;
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
        newface.points[visibledeque[0]] = tempmapface->points[visibledeque[0]];
        newface.UVs[visibledeque[0]] = tempmapface->UVs[visibledeque[0]];
        glm::vec3 newvec3;
        newvec3 = CutLinething(
            temppointsdeque[tempmapface->points[invisibledeque[0]]],
            temppointsdeque[tempmapface->points[visibledeque[0]]]);

        float internal = getinternaldivisionthing(
            temppointsdeque[tempmapface->points[invisibledeque[0]]], newvec3,
            temppointsdeque[tempmapface->points[visibledeque[0]]]);

        newface.UVs[invisibledeque[0]] =
            divisiontoVec2(tempmapface->UVs[invisibledeque[0]],
                           tempmapface->UVs[visibledeque[0]], internal);

        temppointsdeque.push_back(newvec3);

        newface.points[invisibledeque[0]] = temppointsdeque.size() - 1;
        newvec3 = CutLinething(
            temppointsdeque[tempmapface->points[invisibledeque[0]]],
            temppointsdeque[tempmapface->points[visibledeque[1]]]);

        internal = getinternaldivisionthing(
            temppointsdeque[tempmapface->points[invisibledeque[0]]], newvec3,
            temppointsdeque[tempmapface->points[visibledeque[1]]]);

        newface.UVs[visibledeque[1]] =
            divisiontoVec2(tempmapface->UVs[invisibledeque[0]],
                           tempmapface->UVs[visibledeque[1]], internal);

        temppointsdeque.push_back(newvec3);
        tempmapface->points[invisibledeque[0]] = temppointsdeque.size() - 1;
        tempmapface->UVs[invisibledeque[0]] = newface.UVs[visibledeque[1]];
        newface.points[visibledeque[1]] = temppointsdeque.size() - 1;
        newface.shade = tempmapface->shade;
        addlaterfacedeque.push_back(newface);
        break;
      }
    }
  }

  tempmapfacedeque.insert(tempmapfacedeque.end(), addlaterfacedeque.begin(),
                          addlaterfacedeque.end());

  if (Global->rendermode == 0) {
    for (int k = 0; k < tempmapfacedeque.size(); k++) {
      glm::vec3 temp[3] = {temppointsdeque[tempmapfacedeque[k].points[0]],
                           temppointsdeque[tempmapfacedeque[k].points[1]],
                           temppointsdeque[tempmapfacedeque[k].points[2]]};
      glm::vec2 temp2[3] = {tempmapfacedeque[k].UVs[0],
                            tempmapfacedeque[k].UVs[1],
                            tempmapfacedeque[k].UVs[2]};
      DrawTri(tempmapfacedeque[k].texture, temp, temp2,
              tempmapfacedeque[k].xloop, tempmapfacedeque[k].yloop,
              tempmapfacedeque[k].shade);
    }
  } else if (Global->rendermode == 1) {
    for (int k = 0; k < tempmapfacedeque.size(); k++) {
      glm::vec3 temp[2] = {temppointsdeque[tempmapfacedeque[k].points[0]],
                           temppointsdeque[tempmapfacedeque[k].points[1]]};
      DrawLine(10, temp);
      temp[1] = temppointsdeque[tempmapfacedeque[k].points[2]];
      DrawLine(10, temp);
      temp[0] = temppointsdeque[tempmapfacedeque[k].points[1]];
      DrawLine(10, temp);
    }
    // for (int k = 0; k < Global->Points.size(); k++) {
    //   if (Global->editorselectedPoint == k) {
    //     glm::vec3 temp[2] = {Global->Points[k],
    //                          (Global->Points[k] + glm::vec3({0, 4, 0}))};
    //     DrawLine(40, temp);
    //     DrawCircle(40, temp[1], 1);
    //     temp[1] = (Global->Points[k] + glm::vec3({4, 0, 0}));
    //     DrawLine(20, temp);
    //     DrawCircle(20, temp[1], 1);
    //     temp[1] = (Global->Points[k] + glm::vec3({0, 0, 4}));
    //     DrawLine(50, temp);
    //     DrawCircle(50, temp[1], 1);
    //   }
    // }
  }
}

void renderUI() {
  std::deque<UIthing*>* tempdeque = &Global->UImap[Global->UIindex];
  int len = tempdeque->size();
  for (int i = 0; i < len; i++) {
    tempdeque->at(i)->render();
  }
}

void renderbackground() {
  if (Global->rendermode == 0) {
    int x = Settings->resolutionx, y = Settings->resolutiony;
    for (int i = 0; i < x; i++) {
      for (int j = 0; j < y; j++) {
        if (Global->pixelsdepth[i + j * Global->pitch] == 65535 ||
            Global->pixelstransparency[i + j * Global->pitch] < 255) {
          Uint32 color =
              static_cast<Uint32*>(Global->textures[Global->skybox]->pixels)
                  [(int(i * 320.f / x) +
                    int((1 - ((int(Camera->dir.x) % 180) / 180.f)) * 640.f)) %
                       640 +
                   (int((1 - (Camera->dir.y) / 90.f) * 200.f) +
                    int(j * 200.f / y)) *
                       640];
          int r = (color >> 0) & 0xFF;
          int g = (color >> 8) & 0xFF;
          int b = (color >> 16) & 0xFF;
          int a = (color >> 24) & 0xFF;

          if (Global->pixelstransparency[i + j * Global->pitch] < 255) {
            int transparency =
                Global->pixelstransparency[i + j * Global->pitch];
            SDL_Color tempcolor =
                Global->palette->colors[Global->pixels[i + j * Global->pitch]];
            r = r * (255 - transparency) / 255 +
                tempcolor.r * transparency / 255;
            g = g * (255 - transparency) / 255 +
                tempcolor.g * transparency / 255;
            b = b * (255 - transparency) / 255 +
                tempcolor.b * transparency / 255;
          }

          Global->pixels[i + j * Global->pitch] =
              SDL_MapSurfaceRGB(Global->render_target, r, g, b);
        }
      }
    }
  } else if (Global->rendermode == 1) {
    for (int i = 0; i < Settings->resolutionx; i++) {
      for (int j = 0; j < Settings->resolutiony; j++) {
        if (Global->pixelsdepth[i + j * Global->pitch] == 65535) {
          Global->pixels[i + j * Global->pitch] = 1;
        }
      }
    }
  }
}

void render() {
  // SDL_SetRenderDrawColorFloat(Global->renderer, 0, 0, 0, 1);
  // SDL_RenderClear(Global->renderer);
  SDL_LockSurface(Global->render_target);

  Global->pixels = static_cast<unsigned char*>(Global->render_target->pixels);
  Global->pitch = Global->render_target->pitch;

  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      Global->pixelsdepth[i + j * Global->pitch] = 65535;
      Global->pixelstransparency[i + j * Global->pitch] = 255;
    }
  }
  renderUI();
  rendergame();

  renderbackground();

  if (Global->pause || Global->isopeningfile) {
    for (int i = 0; i < Settings->resolutionx; i++) {
      for (int j = 0; j < Settings->resolutiony; j++) {
        Uint8 r, g, b;
        SDL_GetRGB(Global->pixels[i + j * Global->pitch],
                   SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_INDEX8),
                   Global->palette, &r, &g, &b);
        int r2 = r, g2 = g, b2 = b;
        r2 -= 64;
        g2 -= 64;
        b2 -= 64;
        if (r2 < 0) r2 = 0;
        if (g2 < 0) g2 = 0;
        if (b2 < 0) b2 = 0;
        Global->pixels[i + j * Global->pitch] =
            SDL_MapSurfaceRGB(Global->render_target, r2, g2, b2);
      }
    }
  }

  SDL_UnlockSurface(Global->render_target);

  // Screen size and position stuff
  int w = Global->windowx, h = Global->windowy, rtw = Global->render_target->w,
      rth = Global->render_target->h;
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
  SDL_Texture* temptexture =
      SDL_CreateTextureFromSurface(Global->renderer, Global->render_target);
  SDL_SetTextureScaleMode(temptexture, SDL_SCALEMODE_PIXELART);
  SDL_RenderTexture(Global->renderer, temptexture, NULL, &temprect);
  SDL_RenderPresent(Global->renderer);
}