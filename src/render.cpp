#include "render.h"

#include <SDL3/SDL.h>

#include "extern.h"
#include "map.h"

void drawWall(Quad q) {
  Vector3 p1, p2;
  p1 = q.p1 - Camera->position;
  p2 = q.p2 - Camera->position;
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