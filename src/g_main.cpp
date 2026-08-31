#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include <filesystem>

#include "deltaTime.h"
#include "dylib.hpp"
#include "entity.h"
#include "extern.h"
#include "files.h"
#include "global.h"
#include "settings.h"
#include "update.h"

int main(int argc, char* argv[]) {
  std::string basepath = SDL_GetBasePath();
  std::vector<std::string> args;
  args.resize(argc);
  for (int i = 0; i < argc; i++)
    args[i] = argv[i];  // get arguements from command line

  if (!initargs(args)) {  // process arguements
    SDL_Log("%s", SDL_GetError());
    return -1;
  }

  // entity spawning function load from dynamic libraries
  SpawnEntities.reserve(16);
  for (const auto& entry : std::filesystem::directory_iterator(
           basepath + Global->GameName + "/entities/")) {
    if (entry.is_directory()) {
      SDL_Log("Folder: %s", entry.path().filename().string().c_str());
      SpawnEntities[entry.path().filename().string()];
    }
  }
  std::vector<dylib::library> entitylibs;

  for (auto& entry : SpawnEntities) {
    entitylibs.push_back(dylib::library(basepath + "/" + Global->GameName +
                                            "/entities/" + entry.first + "/" +
                                            entry.first,
                                        dylib::decorations::os_default()));
    entry.second = entitylibs.back().get_function<Entity*(uint32_t, uint32_t)>(
        "SpawnEntity");
  }

  // particle spawning function load from dynamic libraries
  SpawnParticles.reserve(16);
  for (const auto& entry : std::filesystem::directory_iterator(
           basepath + Global->GameName + "/particles/")) {
    if (entry.is_directory()) {
      SDL_Log("Folder: %s", entry.path().filename().string().c_str());
      SpawnParticles[entry.path().filename().string()];
    }
  }
  std::vector<dylib::library> particlelibs;

  for (auto& entry : SpawnParticles) {
    particlelibs.push_back(dylib::library(basepath + "/" + Global->GameName +
                                              "/particles/" + entry.first +
                                              "/" + entry.first,
                                          dylib::decorations::os_default()));
    entry.second =
        particlelibs.back().get_function<Particle*(uint32_t, uint32_t)>(
            "SpawnParticle");
  }

  dylib::library UIlib(basepath + "/" + Global->GameName + "/bin/CobblerGameUI",
                       dylib::decorations::os_default());
  SDL_Log("UI library loaded");
  bool (*UIsetup)() = UIlib.get_function<bool()>("UIsetup");
  if (!UIsetup()) {
    SDL_Log("UI load fail");
    return -1;
  }
  SDL_Log("UI loaded");
  void (*changeUIindex)() = UIlib.get_function<void()>("changeUIindex");

  Global->playerclass = "Gardner";
  PlayerClassUpdate.reserve(16);
  for (const auto& entry : std::filesystem::directory_iterator(
           basepath + Global->GameName + "/class/")) {
    if (entry.is_directory()) {
      SDL_Log("Folder: %s", entry.path().filename().string().c_str());
      PlayerClassUpdate[entry.path().filename().string()];
      SpawnEntities[entry.path().filename().string()];
    }
  }

  std::vector<dylib::library> classlibs;
  for (auto& entry : PlayerClassUpdate) {
    classlibs.push_back(dylib::library(basepath + "/" + Global->GameName +
                                           "/class/" + entry.first + "/" +
                                           entry.first,
                                       dylib::decorations::os_default()));
    entry.second = classlibs.back().get_function<void()>("Update");
    SpawnEntities[entry.first] =
        classlibs.back().get_function<Entity*(uint32_t, uint32_t)>(
            "SpawnEntity");
    if (!classlibs.back().get_function<bool()>("UIsetup")()) return -1;
    classlibs.back().get_function<void()>("Init")();
  }
  if (!init()) {
    SDL_Log("%s", SDL_GetError());
    return -1;
  }

  const SDL_DisplayMode* displaymode =
      SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());

  const double refreshrateupdate = displaymode->refresh_rate_denominator /
                                   (double)displaymode->refresh_rate_numerator;
  SDL_Log("%f", refreshrateupdate);
  double ratecountupdate = 0, ratecountrender = 0;
  uint64_t lastTime;
  uint64_t currentTime = SDL_GetPerformanceCounter();
  const double performancefreq = (double)SDL_GetPerformanceFrequency();
  double microdeltatime = 0;

  SDL_Log("Init done");
  while (Global->IsRunning) {
    lastTime = currentTime;
    currentTime = SDL_GetPerformanceCounter();
    microdeltatime = ((double)(currentTime - lastTime)) / performancefreq;
    if (!Settings->vsync) {
      ratecountupdate += microdeltatime;
      ratecountrender += microdeltatime;
    }
    deltaTime += microdeltatime;
    if (Settings->vsync || ratecountupdate >= refreshrateupdate) {
      events();
      input();
      update();
      changeUIindex();

      // SDL_Log("%f %f", ratecountupdate, deltaTime);
      deltaTime = 0;
    }
    if (Settings->vsync || ratecountrender >= (1 / (float)Settings->fps)) {
      render();
      // SDL_Log("%f %f", ratecountrender, 1 / (float)Settings->fps);
    }

    while (ratecountupdate >= refreshrateupdate)
      ratecountupdate -= refreshrateupdate;
    while (ratecountrender >= (1 / (float)Settings->fps))
      ratecountrender -= (1 / (float)Settings->fps);
  }
  if (Global->IsOnline) {
    PlayerQuit();
  }
  for (auto& entry : classlibs) {
    entry.get_function<void()>("Quit")();
  }
  quit();
  void (*UIfree)() = UIlib.get_function<void()>("UIfree");
  UIfree();
  SDL_Log("UIfreed");
  return 0;
}