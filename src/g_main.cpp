#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include <filesystem>
#include <mutex>
#include <thread>

#include "deltaTime.h"
#include "dylib.hpp"
#include "entity.h"
#include "extern.h"
#include "files.h"
#include "global.h"
#include "inputs.h"
#include "settings.h"
#include "update.h"

void (*changeUIindex)();
std::mutex mutexthing;
const double performancefreq = (double)SDL_GetPerformanceFrequency();

void updatefunction() {
  const SDL_DisplayMode* displaymode =
      SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());

  const double refreshrateupdate = displaymode->refresh_rate_denominator /
                                   (double)displaymode->refresh_rate_numerator;

  uint64_t lastTime;
  uint64_t currentTime = SDL_GetPerformanceCounter();
  while (Global->IsRunning) {
    mutexthing.lock();

    lastTime = currentTime;
    currentTime = SDL_GetPerformanceCounter();
    updatedeltaTime = ((double)(currentTime - lastTime)) / performancefreq;
    fixedupdate();
    changeUIindex();
    for (int i = 0; i < 512; i++) {
      if (LocalInputs->Keys[i] > 1) LocalInputs->Keys[i] = 1;
    }
    // If mouse already clicked(2) change value to 1.
    // This allows the game to tell if you've clicked this frame or not.
    if (LocalInputs->leftclick == 2) LocalInputs->leftclick = 1;
    if (LocalInputs->rightclick == 2) LocalInputs->rightclick = 1;

    mutexthing.unlock();

    SDL_DelayPrecise(
        uint64_t((refreshrateupdate -
                  ((double)(SDL_GetPerformanceCounter() - currentTime) /
                   performancefreq)) *
                 1000000000.0));
  }
}

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
  changeUIindex = UIlib.get_function<void()>("changeUIindex");

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

  SDL_Log("Init done");

  std::thread updatethread(updatefunction);

  uint64_t lastTime;
  uint64_t currentTime = SDL_GetPerformanceCounter();
  while (Global->IsRunning) {
    mutexthing.lock();

    lastTime = currentTime;
    currentTime = SDL_GetPerformanceCounter();
    renderdeltaTime = ((double)(currentTime - lastTime)) / performancefreq;
    events();
    input();
    update();
    render();
    mutexthing.unlock();

    renderresult();

    // SDL_Log("%f", 1 / renderdeltaTime);
    if (!Settings->vsync) {
      SDL_DelayPrecise(
          uint64_t(((1 / (double)Settings->fps) -
                    ((double)(SDL_GetPerformanceCounter() - currentTime) /
                     performancefreq)) *
                   1000000000.0));
    }
  }

  updatethread.join();
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