#include "render.h"

#include <SDL3/SDL.h>

#include <cmath>

#include "extern.h"
#include "map.h"

Vector2 drawPoint(Vector3 P) {
  Vector3 p1;
  p1 = P - Camera->position;
  float ps = std::sin(Camera->dir * 3.14 / 180.0);
  float pc = std::cos(Camera->dir * 3.14 / 180.0);

  float tx = p1.x * pc + p1.y * ps;
  float ty = p1.y * pc - p1.x * ps;

  Vector2 screenpos;
  screenpos.x = (tx * Settings->fov / ty) + (Settings->resolutionx / 2);
  screenpos.y = (p1.z * Settings->fov / ty) + (Settings->resolutiony / 2);
  return screenpos;
}

void render() {
  SDL_SetRenderDrawColorFloat(Global->renderer, 0, 0, 0, 1);
  SDL_RenderClear(Global->renderer);
  SDL_SetRenderTarget(Global->renderer, Global->render_target);

  Quad tempquad;
  tempquad.p1 = Vector3({-1, 1, -2});
  tempquad.p2 = Vector3({1, 1, -1});
  Vector2 temp = drawPoint(tempquad.p1), temp2 = drawPoint(tempquad.p2);
  SDL_SetRenderDrawColorFloat(Global->renderer, 0, 1, 0, 1);

  SDL_RenderLine(Global->renderer, temp.x, temp.y, temp2.x, temp2.y);
  // SDL_Log("%f %f %f %f", temp.x, temp.y, temp2.x, temp2.y);

  SDL_SetRenderTarget(Global->renderer, NULL);
  SDL_RenderTexture(Global->renderer, Global->render_target, NULL, NULL);
  SDL_RenderPresent(Global->renderer);
}