#include "render.h"

#include <SDL3/SDL.h>

#include <cmath>
#include <string>

#include "extern.h"
#include "map.h"

struct ScreenPoint {
  Vector2 p;
  bool isbehindcamera = false;
};

ScreenPoint drawPoint(Vector3 P) {
  Vector3 p1;
  for (int i = 0; i < 3; i++) {
    p1.v[i] = P.v[i] - Camera->position.v[i];
  }
  float ps = std::sin(Camera->dir.x * 3.14 / 180.0);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.0);

  float tx = p1.x * pc + p1.y * ps;
  float ty = p1.y * pc - p1.x * ps;

  ScreenPoint screenpos;
  if (ty <= 0.f) {
    screenpos.isbehindcamera = true;
    ty = 1.f / 64.f;
  }
  screenpos.p.x = (tx * Settings->fov / ty) + (Settings->resolutionx / 2);
  screenpos.p.y = (-p1.z * Settings->fov / ty) + (Settings->resolutiony / 2);
  return screenpos;
}

void DrawLine(unsigned char* pixels, int pitch, unsigned char color,
              ScreenPoint vectors[]) {
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

void DrawQuad(unsigned char* pixels, int pitch, std::string texture,
              ScreenPoint vectors[]) {
  if (!vectors[0].isbehindcamera || !vectors[1].isbehindcamera ||
      !vectors[2].isbehindcamera || !vectors[3].isbehindcamera) {
    int x = vectors[0].p.x, x2 = vectors[0].p.x, y = vectors[0].p.y,
        y2 = vectors[0].p.y;
    for (int i = 1; i < 4; i++) {
      if (vectors[i].p.x < x) x = vectors[i].p.x;
      if (vectors[i].p.x > x2) x2 = vectors[i].p.x;
      if (vectors[i].p.y < y) y = vectors[i].p.y;
      if (vectors[i].p.y > y2) y2 = vectors[i].p.y;
    }
    if (x < 0) x = 0;
    if (x >= Settings->resolutionx) x = Settings->resolutionx;
    if (x2 < 0) x2 = 0;
    if (x2 >= Settings->resolutionx) x2 = Settings->resolutionx;
    if (y < 0) y = 0;
    if (y >= Settings->resolutiony) y = Settings->resolutiony;
    if (y2 < 0) y2 = 0;
    if (y2 >= Settings->resolutiony) y2 = Settings->resolutiony;
    for (int i = x; i < x2; i++) {
      for (int j = y; j < y2; j++) {
        Vector2 temp;
        temp.x = i;
        temp.y = j;
        if (temp.x >= 0 && temp.y >= 0 && temp.x <= Settings->resolutionx &&
            temp.y <= Settings->resolutiony &&
            (Vector2inTri(temp, vectors[0].p, vectors[1].p, vectors[2].p) ||
             Vector2inTri(temp, vectors[0].p, vectors[2].p, vectors[3].p))) {
          unsigned char color = static_cast<unsigned char*>(
              Global->texturemap.at(texture)->pixels)[i + j * pitch];
          pixels[i + j * pitch] = color;
        }
      }
    }
  }
}

void DrawQuad(unsigned char* pixels, int pitch, unsigned char color,
              ScreenPoint vectors[]) {
  if (!vectors[0].isbehindcamera || !vectors[1].isbehindcamera ||
      !vectors[2].isbehindcamera || !vectors[3].isbehindcamera) {
    if (color == 255) {
      ScreenPoint temp[2] = {vectors[0], vectors[1]};
      DrawLine(pixels, pitch, color, temp);
      temp[0] = vectors[1];
      temp[1] = vectors[2];
      DrawLine(pixels, pitch, color, temp);
      temp[0] = vectors[3];
      temp[1] = vectors[2];
      DrawLine(pixels, pitch, color, temp);
      temp[0] = vectors[0];
      temp[1] = vectors[3];
      DrawLine(pixels, pitch, color, temp);
      temp[0] = vectors[0];
      temp[1] = vectors[2];
      DrawLine(pixels, pitch, color, temp);
      temp[0] = vectors[3];
      temp[1] = vectors[1];
      DrawLine(pixels, pitch, color, temp);
    } else {
      int x = vectors[0].p.x, x2 = vectors[0].p.x, y = vectors[0].p.y,
          y2 = vectors[0].p.y;
      for (int i = 1; i < 4; i++) {
        if (vectors[i].p.x < x) x = vectors[i].p.x;
        if (vectors[i].p.x > x2) x2 = vectors[i].p.x;
        if (vectors[i].p.y < y) y = vectors[i].p.y;
        if (vectors[i].p.y > y2) y2 = vectors[i].p.y;
      }
      if (x < 0) x = 0;
      if (x >= Settings->resolutionx) x = Settings->resolutionx;
      if (x2 < 0) x2 = 0;
      if (x2 >= Settings->resolutionx) x2 = Settings->resolutionx;
      if (y < 0) y = 0;
      if (y >= Settings->resolutiony) y = Settings->resolutiony;
      if (y2 < 0) y2 = 0;
      if (y2 >= Settings->resolutiony) y2 = Settings->resolutiony;
      for (int i = x; i < x2; i++) {
        for (int j = y; j < y2; j++) {
          Vector2 temp;
          temp.x = i;
          temp.y = j;
          if (temp.x >= 0 && temp.y >= 0 && temp.x <= Settings->resolutionx &&
              temp.y <= Settings->resolutiony &&
              (Vector2inTri(temp, vectors[0].p, vectors[1].p, vectors[2].p) ||
               Vector2inTri(temp, vectors[0].p, vectors[2].p, vectors[3].p)))
            pixels[i + j * pitch] = color;
        }
      }
    }
  }
}

void render() {
  // SDL_SetRenderDrawColorFloat(Global->renderer, 0, 0, 0, 1);
  // SDL_RenderClear(Global->renderer);
  SDL_LockSurface(Global->render_target);

  Quad tempquad;
  tempquad.p1 = Vector3({-1, 1, 2});
  tempquad.p2 = Vector3({1, 1, 2});
  tempquad.p3 = Vector3({1, 1, -2});
  tempquad.p4 = Vector3({-1, 1, -2});
  ScreenPoint temp[4] = {drawPoint(tempquad.p1), drawPoint(tempquad.p2),
                         drawPoint(tempquad.p3), drawPoint(tempquad.p4)};

  unsigned char* pixels =
      static_cast<unsigned char*>(Global->render_target->pixels);
  int pitch = Global->render_target->pitch;

  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      pixels[i + j * pitch] = 0;
    }
  }

  unsigned char color = SDL_MapSurfaceRGB(Global->render_target, 0, 255, 0);
  DrawQuad(pixels, pitch, "Wall", temp);

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