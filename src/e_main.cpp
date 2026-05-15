#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include <string>

#include "entity.h"
#include "extern.h"
#include "files.h"
#include "global.h"
#include "ui.h"
#include "update.h"

bool editorinit() {
  Editor = new EditorClass();
  if (Editor == nullptr) return false;
  if (!UIsetup()) return false;
  return true;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args;
  args.resize(argc);
  for (int i = 0; i < argc; i++) args[i] = argv[i];

  if (!init(true, args) || !editorinit()) {
    SDL_Log("%s", SDL_GetError());
    return -1;
  }
  Global->IsEditor = true;
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