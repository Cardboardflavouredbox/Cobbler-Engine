#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include <filesystem>

#include "dylib.hpp"
#include "entity.h"
#include "extern.h"
#include "files.h"
#include "global.h"
#include "update.h"

int main(int argc, char* argv[]) {
  std::string basepath = SDL_GetBasePath();
  std::vector<std::string> args;
  args.resize(argc);
  for (int i = 0; i < argc; i++) args[i] = argv[i];

  if (!initargs(args)) {
    SDL_Log("%s", SDL_GetError());
    return -1;
  }

  SpawnEntities.reserve(16);
  for (const auto& entry : std::filesystem::directory_iterator(
           basepath + Global->GameName + "/entities/")) {
    if (entry.is_directory()) {
      SDL_Log("Folder: %s", entry.path().filename().c_str());
      SpawnEntities[entry.path().filename().string()];
    }
  }
  std::vector<dylib::library> entitylibs;

  for (auto& entry : SpawnEntities) {
    entitylibs.push_back(dylib::library(basepath + "/" + Global->GameName +
                                            "/entities/" + entry.first + "/" +
                                            entry.first,
                                        dylib::decorations::os_default()));
    entry.second = entitylibs.back().get_function<Entity*()>("SpawnEntity");
  }

  if (!init(false)) {
    SDL_Log("%s", SDL_GetError());
    return -1;
  }

  dylib::library lib(basepath + "/" + Global->GameName + "/bin/CobblerGameUI",
                     dylib::decorations::os_default());
  SDL_Log("UI library loaded");
  bool (*UIsetup)() = lib.get_function<bool()>("UIsetup");
  if (!UIsetup()) {
    SDL_Log("UI load fail");
    return -1;
  }
  SDL_Log("UI loaded");
  void (*changeUIindex)() = lib.get_function<void()>("changeUIindex");

  Global->IsEditor = false;
  SDL_Log("Init done");
  while (Global->IsRunning) {
    Uint64 start = SDL_GetPerformanceCounter();
    events();
    input();
    update();
    changeUIindex();
    render();
    Uint64 result = (SDL_GetPerformanceCounter() - start);
    if (!Settings->vsync && result < 1000000000 / (double)Settings->fps) {
      SDL_DelayNS(1000000000 / (double)Settings->fps - result);
    }
  }
  quit();
  void (*UIfree)() = lib.get_function<void()>("UIfree");
  UIfree();
  SDL_Log("UIfreed");
  return 0;
}