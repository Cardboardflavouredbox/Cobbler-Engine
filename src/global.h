#pragma once

#include <SDL3/SDL_render.h>

struct Global {
 private:
  Global() {}
  Global(const Global& ref) {}
  Global& operator=(const Global& ref) {}
  ~Global() {}

 public:
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* render_target;
  bool IsRunning;
  static Global& GetGlobal() {
    static Global s;
    return s;
  }
};