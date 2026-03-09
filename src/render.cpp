#include "render.h"

#include <SDL3/SDL.h>

#include "extern.h"

void render() {
  SDL_RenderClear(Global->renderer);
  SDL_SetRenderTarget(Global->renderer, Global->render_target);
  SDL_SetRenderDrawColorFloat(Global->renderer, 0, 1, 0, 1);
  SDL_RenderLine(Global->renderer, 0, 0, 1, 1);
  SDL_RenderPresent(Global->renderer);
}