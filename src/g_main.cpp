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

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "deltaTime.h"
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
  while (IsRunning) {
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
  // Your Global is running? You better go catch it.
  IsRunning = true;

  std::vector<std::string> args;
  args.resize(argc);
  for (int i = 0; i < argc; i++)
    args[i] = argv[i];  // get arguements from command line

  if (!initargs(args)) {  // process arguements
    SDL_Log("%s", SDL_GetError());
    return -1;
  }

  SDL_Log("UI library loaded");

  SDL_Log("UI loaded");

  Global->playerclass = "Gardner";

  if (!init()) {
    SDL_Log("%s", SDL_GetError());
    return -1;
  }

  SDL_Log("Init done");

  std::thread updatethread(updatefunction);

  uint64_t lastTime;
  uint64_t currentTime = SDL_GetPerformanceCounter();
  while (IsRunning) {
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

  quit();
  SDL_Log("UIfreed");
  return 0;
}