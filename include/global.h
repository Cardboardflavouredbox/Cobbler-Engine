#pragma once

#include <SDL3/SDL_render.h>

struct GlobalClass {
 public:
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* render_target;
  bool IsRunning;
};

struct Inputs {
 public:
  unsigned char W = 0, A = 0, S = 0, D = 0;
};

struct SettingsClass {
 public:
  uint16_t resolutionx, resolutiony;
  int fov = 90;
};