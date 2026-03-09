#include "render.h"

#include <SDL3/SDL.h>

#include "global.h"

void render() {
  SDL_RenderClear(Global::GetGlobal().renderer);
  SDL_SetRenderTarget(Global::GetGlobal().renderer, Global::GetGlobal().render_target);
  SDL_SetRenderDrawColorFloat(Global::GetGlobal().renderer, 0, 1, 0, 1);
  SDL_RenderLine(Global::GetGlobal().renderer, 0,0,1,1);
  SDL_RenderPresent(Global::GetGlobal().renderer);
}