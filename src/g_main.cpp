#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include <string>

#include "entity.h"
#include "files.h"
#include "global.h"
#include "update.h"

GlobalClass* Global;
EditorClass* Editor;
SettingsClass* Settings;
ZipData* LoadedData;
Entity* Camera;
Inputs* P1Inputs;
Uint32 lastTime;
Uint64 currentTime = SDL_GetPerformanceCounter();

bool gameinit() {
  Global->UImap.reserve(4);
  std::deque<UIthing*> tempdeque;
  UIimage* weapon = new UIimage();
  weapon->color = 11;
  weapon->pos = glm::vec2({160 - 48, 200 - 112});
  weapon->size = glm::vec2({96, 112});
  weapon->uvlist =
      new std::pair<glm::vec2, glm::vec2>[3]{{{0, 0}, {1 / 3.f, 1.f}},
                                             {{1 / 3.f, 0}, {2 / 3.f, 1.f}},
                                             {{2 / 3.f, 0}, {1.f, 1.f}}};
  weapon->textureindex = 4;
  tempdeque.push_back(weapon);
  Global->UImap[0] = tempdeque;
  return true;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args;
  args.resize(argc);
  for (int i = 0; i < argc; i++) args[i] = argv[i];

  if (!init(false, args) || !gameinit()) {
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
    if (!Settings->vsync && result < 1000000000 / (double)Settings->fps) {
      SDL_DelayNS(1000000000 / (double)Settings->fps - result);
    }
  }
  quit();
  return 0;
}