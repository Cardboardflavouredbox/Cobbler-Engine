#include "render.h"

#include <SDL3/SDL.h>
#include <math.h>

#include "extern.h"
#include "map.h"

void drawPoint(Vector P) {
  Vector3 p1;
  p1 = P - Camera->position;
  float ps = std::sin(Camera->dir * 3.14 / 180.0);
  float pc = std::cos(Camera->dir * 3.14 / 180.0);

  float tx = p1.x * pc + p1.y * ps;
  float ty = p1.y * pc - p1.x * ps;

  float screenX = (tx * Settings->fov / ty) + (Settings->resolutionx / 2);
  float screenHeight = (1 * Settings->fov / ty);
}

void render() {
  SDL_RenderClear(Global->renderer);
  SDL_SetRenderTarget(Global->renderer, Global->render_target);
  SDL_SetRenderDrawColorFloat(Global->renderer, 0, 1, 0, 1);
  SDL_RenderLine(Global->renderer, 0, 0, 1, 1);
  SDL_SetRenderTarget(Global->renderer, NULL);
  SDL_RenderTexture(Global->renderer, Global->render_target, NULL, NULL);
  SDL_RenderPresent(Global->renderer);
}