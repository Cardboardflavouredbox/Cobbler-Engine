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
  if (!init(false)) {
    SDL_Log("%s", SDL_GetError());
    return -1;
  }
  SDL_Log("Init done");
  while (Global->IsRunning) {
    events();
    input();
    update();
    render();
  }
  quit();
  return 0;
}