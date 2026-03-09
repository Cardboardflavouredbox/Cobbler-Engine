#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include "entity.h"
#include "global.h"
#include "render.h"
/* We will use this renderer to draw into this window every frame. */
GlobalClass* Global;
SettingsClass* Settings;
Entity* Camera;

bool init() {
  Global = static_cast<GlobalClass*>(calloc(1, sizeof(GlobalClass)));
  Settings = static_cast<SettingsClass*>(calloc(1, sizeof(SettingsClass)));
  Settings->resolutionx = 320;
  Settings->resolutiony = 200;

  Camera = static_cast<Entity*>(calloc(1, sizeof(Entity)));

  if (!SDL_SetAppMetadata("BoomerShooter", "0.1", "com.example.myapp") ||
      !SDL_Init(SDL_INIT_VIDEO))
    return false;
  Global->window = SDL_CreateWindow("SDL3 window", Settings->resolutionx,
                                    Settings->resolutiony, 0);
  Global->renderer = SDL_CreateRenderer(Global->window, NULL);
  Global->render_target = SDL_CreateTexture(
      Global->renderer, SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_TARGET,
      Settings->resolutionx, Settings->resolutiony);
  Global->IsRunning = true;
  return true;
}
void quit() {
  SDL_DestroyRenderer(Global->renderer);
  SDL_DestroyWindow(Global->window);
  Global->IsRunning = false;
  SDL_Quit();
}
void input() {}

void update() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        Global->IsRunning = false;
        break;
    }
  }
}

int main(int argc, char* argv[]) {
  if (!init()) {
    SDL_Log(SDL_GetError());
    return -1;
  }
  while (Global->IsRunning) {
    input();
    update();
    render();
  }
  quit();
  return 0;
}