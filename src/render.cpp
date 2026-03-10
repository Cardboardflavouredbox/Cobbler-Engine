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

void render() {
  SDL_SetRenderDrawColorFloat(Global->renderer, 0, 0, 0, 1);
  SDL_RenderClear(Global->renderer);
  SDL_LockSurface(Global->render_target);

  Quad tempquad;
  tempquad.p1 = Vector3({-1, 1, 2});
  tempquad.p2 = Vector3({1, 1, 2});
  tempquad.p3 = Vector3({1, 1, -2});
  tempquad.p4 = Vector3({-1, 1, -2});
  Vector2 temp = drawPoint(tempquad.p1), temp2 = drawPoint(tempquad.p2),
          temp3 = drawPoint(tempquad.p3), temp4 = drawPoint(tempquad.p4);

  uint32_t* pixels = static_cast<uint32_t*>(Global->render_target->pixels);

  for (int i = temp.x; i <= temp2.x; i++) pixels[i] = uint32_t(0);
  SDL_UnlockSurface(Global->render_target);

  int w, h;
  SDL_GetWindowSizeInPixels(Global->window, &w, &h);
  SDL_Log(SDL_GetError());
  w /= 2;
  h /= 2;
  w -= Global->render_target->w / 2;
  h -= Global->render_target->h / 2;

  SDL_FRect temprect;
  temprect.w = Global->render_target->w;
  temprect.h = Global->render_target->h;
  temprect.x = w;
  temprect.y = h;

  SDL_RenderTexture(
      Global->renderer,
      SDL_CreateTextureFromSurface(Global->renderer, Global->render_target),
      NULL, &temprect);
  SDL_RenderPresent(Global->renderer);
}