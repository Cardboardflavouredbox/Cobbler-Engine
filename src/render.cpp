#include "render.h"

#include <SDL3/SDL.h>

#include <cmath>
#include <deque>
#include <string>

#include "extern.h"
#include "map.h"

struct ScreenPoint {
  Vector2 p;
  float dist;
  bool isbehindcamera = false;
};

float getdistancething(Vector3 P) {
  Vector3 p1;
  p1.x = P.x - Camera->position.x;
  p1.y = P.y - Camera->position.y;
  p1.z = P.z - Camera->position.z;
  float ps = std::sin(Camera->dir.x * 3.14 / 180.f);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.f);
  float what = std::sin(Camera->dir.y * 3.14 / 180.f);
  return p1.y * pc - p1.x * ps + p1.z * what;
}

ScreenPoint drawPoint(Vector3 P) {
  Vector3 p1;
  p1.x = P.x - Camera->position.x;
  p1.y = P.y - Camera->position.y;
  p1.z = P.z - Camera->position.z;

  float ps = std::sin(Camera->dir.x * 3.14 / 180.f);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.f);
  float what = std::sin(Camera->dir.y * 3.14 / 180.f);

  float tx = p1.x * pc + p1.y * ps;
  float ty = p1.y * pc - p1.x * ps + p1.z * what;
  float tz = 180 * what;

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

float Vector2Dot(Vector2 P1, Vector2 P2) {
  float deltaX = P2.x - P1.x;
  float deltaY = P2.y - P1.y;
  return std::sqrt(deltaX * deltaX + deltaY * deltaY);
}

float Areathing(Vector2 a, Vector2 b, Vector2 c) {
  return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}
Vector3 GetUV(Vector2 P, ScreenPoint R1, ScreenPoint R2, ScreenPoint R3) {
  float det = Areathing(R1.p, R2.p, R3.p);
  float a[3] = {Areathing(R2.p, R3.p, P), Areathing(R3.p, R1.p, P),
                Areathing(R1.p, R2.p, P)};
  for (int i = 0; i < 3; i++) a[i] /= det;
  return Vector3({a[0], a[1], a[2]});
}
void DrawLine(unsigned char* pixels, int pitch, unsigned char color,
              Vector3 rawvectors[]) {
  ScreenPoint vectors[2] = {drawPoint(rawvectors[0]), drawPoint(rawvectors[1])};
  if (!vectors[0].isbehindcamera || !vectors[1].isbehindcamera) {
    for (int i = vectors[0].p.x; i < vectors[1].p.x; i++) {
      int y = vectors[0].p.y +
              ((i - vectors[0].p.x) * (vectors[1].p.y - vectors[0].p.y) /
               (vectors[1].p.x - vectors[0].p.x));
      if (i >= 0 && y >= 0 && i <= Settings->resolutionx &&
          y <= Settings->resolutiony) {
        pixels[i + y * pitch] = color;
      }
    }
    if (vectors[0].p.x == vectors[1].p.x) {
      for (int i = vectors[0].p.y; i < vectors[1].p.y; i++) {
        if (vectors[0].p.x >= 0 && i >= 0 &&
            vectors[0].p.x <= Settings->resolutionx &&
            i <= Settings->resolutiony) {
          pixels[(int)vectors[0].p.x + i * pitch] = color;
        }
      }
    }
  }
}

float anglething(Vector2 a, Vector2 b, Vector2 c) {
  Vector2 ab = Vector2({b.x - a.x, b.y - a.y});
  Vector2 cb = Vector2({b.x - c.x, b.y - c.y});

  float dot = (ab.x * cb.x + ab.y * cb.y);
  float cross = (ab.x * cb.y - ab.y * cb.x);

  float alpha = atan2f(cross, dot);

  return alpha * 180.f / 3.14f + 0.5f;
}
float Vector2inTri(Vector2 p, Vector2 v1, Vector2 v2, Vector2 v3) {
  float s1 = anglething(v3, p, v1), s2 = anglething(v1, p, v2),
        s3 = anglething(v2, p, v3);

  return (s1 + s2 + s3 > 360);
}

void DrawTri(unsigned char* pixels, int pitch, int texture,
             Vector3 rawvectors[], Vector2 UVs[], int xloop, int yloop) {
  ScreenPoint vectors[3] = {drawPoint(rawvectors[0]), drawPoint(rawvectors[1]),
                            drawPoint(rawvectors[2])};
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
        Vector2 temp;
        temp.x = i;
        temp.y = j;
        if (temp.x >= 0 && temp.y >= 0 && temp.x < Settings->resolutionx &&
            temp.y < Settings->resolutiony) {
          if (Vector2inTri(temp, Vector2({vectors[0].p.x, vectors[0].p.y}),
                           Vector2({vectors[1].p.x, vectors[1].p.y}),
                           Vector2({vectors[2].p.x, vectors[2].p.y}))) {
            Vector3 uvw = GetUV(temp, vectors[0], vectors[1], vectors[2]);
            Vector2 uvresult = addVec2(
                addVec2(
                    multiplyVec2(multiplyVec2(UVs[0], uvw.x), vectors[0].dist),
                    multiplyVec2(multiplyVec2(UVs[1], uvw.y), vectors[1].dist)),
                multiplyVec2(multiplyVec2(UVs[2], uvw.z), vectors[2].dist));
            uvresult = multiplyVec2(uvresult, (1 / (uvw.x * vectors[0].dist +
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

            Vector3 tempvec3;
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
            r -= dist * 4;
            g -= dist * 4;
            b -= dist * 4;
            // r = uvresult.x * 255;
            // g = uvresult.y * 255;
            // b = 0;
            if (r < 0) r = 0;
            if (g < 0) g = 0;
            if (b < 0) b = 0;
            pixels[i + j * pitch] =
                SDL_MapSurfaceRGB(Global->render_target, r, g, b);
          }
        }
      }
    }
  }
}

float getinternaldivisionthing(Vector3 p1, Vector3 d, Vector3 p2) {
  float dist1 = getVec3dist(p1, d), dist2 = getVec3dist(d, p2);
  return dist1 / (dist1 + dist2);
}

Vector2 divisiontoVec2(Vector2 p1, Vector2 p2, float t) {
  return Vector2({p2.x * t + p1.x * (1 - t), p2.y * t + p1.y * (1 - t)});
}

Vector3 CutLinething(Vector3 invisible, Vector3 visible) {
  Vector3 p1, p2;
  p1.x = invisible.x - Camera->position.x;
  p1.y = invisible.y - Camera->position.y;
  p1.z = invisible.z - Camera->position.z;
  p2.x = visible.x - Camera->position.x;
  p2.y = visible.y - Camera->position.y;
  p2.z = visible.z - Camera->position.z;
  float ps = std::sin(Camera->dir.x * 3.14 / 180.f);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.f);
  float what = std::sin(Camera->dir.y * 3.14 / 180.f);

  float u = (p1.y * pc - p1.x * ps + p1.z * what - 0.25f) /
            (-ps * (p1.x - p2.x) + pc * (p1.y - p2.y) + what * (p1.z - p2.z));

  Vector3 result =
      addVec3(invisible, multiplyVec3(subVec3(visible, invisible), u));
  return result;
}

void rendergame(unsigned char* pixels, int pitch) {
  std::deque<Mapface> tempmapfacedeque = Global->mapfaces, addlaterfacedeque;
  std::deque<Vector3> temppointsdeque = Global->Points;

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
          Vector3 newvec3 = CutLinething(
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
        Vector3 newvec3;
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
        addlaterfacedeque.push_back(newface);
        break;
      }
    }
  }

  tempmapfacedeque.insert(tempmapfacedeque.end(), addlaterfacedeque.begin(),
                          addlaterfacedeque.end());

  for (int k = 0; k < tempmapfacedeque.size(); k++) {
    Vector3 temp[3] = {temppointsdeque[tempmapfacedeque[k].points[0]],
                       temppointsdeque[tempmapfacedeque[k].points[1]],
                       temppointsdeque[tempmapfacedeque[k].points[2]]};
    Vector2 temp2[3] = {tempmapfacedeque[k].UVs[0], tempmapfacedeque[k].UVs[1],
                        tempmapfacedeque[k].UVs[2]};
    DrawTri(pixels, pitch, tempmapfacedeque[k].texture, temp, temp2,
            tempmapfacedeque[k].xloop, tempmapfacedeque[k].yloop);
  }
}

void render() {
  // SDL_SetRenderDrawColorFloat(Global->renderer, 0, 0, 0, 1);
  // SDL_RenderClear(Global->renderer);
  SDL_LockSurface(Global->render_target);

  unsigned char* pixels =
      static_cast<unsigned char*>(Global->render_target->pixels);
  int pitch = Global->render_target->pitch;

  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      Uint32 color = static_cast<Uint32*>(
          Global->textures[Global->skybox]->pixels)[i + j * 640];
      int r = (color >> 0) & 0xFF;
      int g = (color >> 8) & 0xFF;
      int b = (color >> 16) & 0xFF;
      int a = (color >> 24) & 0xFF;
      pixels[i + j * pitch] = SDL_MapSurfaceRGB(Global->render_target, r, g, b);
    }
  }

  rendergame(pixels, pitch);

  if (Global->pause) {
    for (int i = 0; i < Settings->resolutionx; i++) {
      for (int j = 0; j < Settings->resolutiony; j++) {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i + j * pitch],
                   SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_INDEX8),
                   Global->palette, &r, &g, &b);
        int r2 = r, g2 = g, b2 = b;
        r2 -= 64;
        g2 -= 64;
        b2 -= 64;
        if (r2 < 0) r2 = 0;
        if (g2 < 0) g2 = 0;
        if (b2 < 0) b2 = 0;
        pixels[i + j * pitch] =
            SDL_MapSurfaceRGB(Global->render_target, r2, g2, b2);
      }
    }
  }

  SDL_UnlockSurface(Global->render_target);
  int w, h, rtw = Global->render_target->w, rth = Global->render_target->h;
  SDL_GetWindowSizeInPixels(Global->window, &w, &h);

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