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

bool editorinit() {
  Global->UImap.reserve(4);
  std::deque<UIthing*> tempdeque;
  UIbox* temp = new UIbox();
  temp->color = 12;
  temp->pos = Vector2({4, 4});
  temp->size = Vector2({16, 64});
  tempdeque.push_back(temp);
  Global->UImap[0] = tempdeque;
  return true;
}

int main(int argc, char* argv[]) {
  if (!init(false) || !editorinit()) {
    SDL_Log("%s", SDL_GetError());
    return -1;
  }
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