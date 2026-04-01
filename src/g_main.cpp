#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include <string>

#include "entity.h"
#include "files.h"
#include "global.h"
#include "update.h"

GlobalClass* Global;
SettingsClass* Settings;
ZipData* LoadedData;
Entity* Camera;
Inputs* P1Inputs;
Uint32 lastTime;
Uint64 currentTime = SDL_GetPerformanceCounter();

int main(int argc, char* argv[]) {
  if (!init(true)) {
    SDL_Log("%s", SDL_GetError());
    return -1;
  }
  Global->IsEditor = false;
  SDL_Log("Init done");
  while (Global->IsRunning) {
    Uint64 start = SDL_GetPerformanceCounter();
    events();
    input();
    update();
    render();
    Uint64 result = (SDL_GetPerformanceCounter() - start);
    if (result < 1000000000 / (double)Settings->fps) {
      SDL_DelayNS(1000000000 / (double)Settings->fps - result);
    }
  }
  quit();
  return 0;
}