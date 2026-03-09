#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "global.h"
#include "render.h"
/* We will use this renderer to draw into this window every frame. */

bool init() {
  if (!SDL_SetAppMetadata("BoomerShooter", "0.1", "com.example.myapp") ||
      !SDL_Init(SDL_INIT_VIDEO))
    return false;
  Global::GetGlobal().window = SDL_CreateWindow("SDL3 window", 320, 200, 0);
  Global::GetGlobal().renderer =
      SDL_CreateRenderer(Global::GetGlobal().window, NULL);
  Global::GetGlobal().render_target =
      SDL_CreateTexture(Global::GetGlobal().renderer, SDL_PIXELFORMAT_INDEX8,
                        SDL_TEXTUREACCESS_TARGET, 320, 200);
  Global::GetGlobal().IsRunning = true;
  return true;
}
void quit() {
  SDL_DestroyRenderer(Global::GetGlobal().renderer);
  SDL_DestroyWindow(Global::GetGlobal().window);
  Global::GetGlobal().IsRunning = false;
  SDL_Quit();
}
void input() {}

void update() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        Global::GetGlobal().IsRunning = false;
        break;
    }
  }
}



int main(int argc, char* argv[]) {
  if (!init()) {
    SDL_Log(SDL_GetError());
    return -1;
  }
  while (Global::GetGlobal().IsRunning) {
    input();
    update();
    render();
  }
  quit();
  return 0;
}