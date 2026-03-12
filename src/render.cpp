#include "render.h"

#include <SDL3/SDL.h>

#include <cmath>

#include "extern.h"
#include "map.h"

Vector2 drawPoint(Vector3 P) {
  Vector3 p1;
  for (int i = 0; i < 3; i++) {
    p1.v[i] = P.v[i] - Camera->position.v[i];
  }
  float ps = std::sin(Camera->dir * 3.14 / 180.0);
  float pc = std::cos(Camera->dir * 3.14 / 180.0);

  float tx = p1.x * pc + p1.y * ps;
  float ty = p1.y * pc - p1.x * ps;

  if (ty <= 0) ty = 1 / 256.f;

  Vector2 screenpos;
  screenpos.x = (tx * Settings->fov / ty) + (Settings->resolutionx / 2);
  screenpos.y = (-p1.z * Settings->fov / ty) + (Settings->resolutiony / 2);
  return screenpos;
}

void DrawLine(unsigned char* pixels, int pitch, unsigned char color,
              Vector2 vectors[]) {
  for (int i = vectors[0].x; i < vectors[1].x; i++) {
    int y = vectors[0].y + ((i - vectors[0].x) * (vectors[1].y - vectors[0].y) /
                            (vectors[1].x - vectors[0].x));
    if (i >= 0 && y >= 0 && i <= Settings->resolutionx &&
        y <= Settings->resolutiony) {
      pixels[i + y * pitch] = color;
    }
  }
  if (vectors[0].x == vectors[1].x) {
    for (int i = vectors[0].y; i < vectors[1].y; i++) {
      if (vectors[0].x >= 0 && i >= 0 &&
          vectors[0].x <= Settings->resolutionx && i <= Settings->resolutiony) {
        pixels[(int)vectors[0].x + i * pitch] = color;
      }
    }
  }
}

bool Vector2inTri(Vector2 p, Vector2 v1, Vector2 v2, Vector2 v3) {
  float N = atan2f(p.y - v1.y, p.x - v1.x),
        s1 = atan2f(v1.y - v2.y, v1.x - v2.x),
        s2 = atan2f(v1.y - v3.y, v1.x - v3.x);
  if (N < 0) N += 3.14f;
  if (s1 < 0) s1 += 3.14f;
  if (s2 < 0) s2 += 3.14f;
  if (s1 > s2) {
    float temp = s1;
    s1 = s2;
    s2 = temp;
  }
  if (s1 > N || N > s2) return false;
  N = atan2f(p.y - v2.y, p.x - v2.x);
  s1 = atan2f(v1.y - v2.y, v1.x - v2.x);
  s2 = atan2f(v2.y - v3.y, v2.x - v3.x);
  if (N < 0) N += 3.14f;
  if (s1 < 0) s1 += 3.14f;
  if (s2 < 0) s2 += 3.14f;
  if (s1 > s2) {
    float temp = s1;
    s1 = s2;
    s2 = temp;
  }
  if (s1 > N || N > s2) return false;

  return true;
}

void DrawQuad(unsigned char* pixels, int pitch, unsigned char color,
              Vector2 vectors[], bool wire) {
  if (wire) {
    Vector2 temp[2] = {vectors[0], vectors[1]};
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
    int x = vectors[0].x, x2 = vectors[0].x, y = vectors[0].y,
        y2 = vectors[0].y;
    for (int i = 1; i < 4; i++) {
      if (vectors[i].x < x) x = vectors[i].x;
      if (vectors[i].x > x2) x2 = vectors[i].x;
      if (vectors[i].y < y) y = vectors[i].y;
      if (vectors[i].y > y2) y2 = vectors[i].y;
    }
    if (x < 0) x = 0;
    if (x2 >= Settings->resolutionx) x2 = Settings->resolutionx;
    if (y < 0) y = 0;
    if (y2 >= Settings->resolutiony) y2 = Settings->resolutiony;
    for (int i = x; i < x2; i++) {
      for (int j = y; j < y2; j++) {
        Vector2 temp;
        temp.x = i;
        temp.y = j;
        if (temp.x >= 0 && temp.y >= 0 && temp.x <= Settings->resolutionx &&
            temp.y <= Settings->resolutiony &&
            (Vector2inTri(temp, vectors[0], vectors[1], vectors[2]) ||
             Vector2inTri(temp, vectors[0], vectors[3], vectors[2])))
          pixels[i + j * pitch] = color;
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
  Vector2 temp[4] = {drawPoint(tempquad.p1), drawPoint(tempquad.p2),
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
  DrawQuad(pixels, pitch, color, temp, false);

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