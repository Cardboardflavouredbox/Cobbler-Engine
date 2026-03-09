#pragma once

#include <SDL3/SDL_render.h>

struct GlobalClass {
 public:
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* render_target;
  bool IsRunning;
};

struct SettingsClass {
 public:
  uint16_t resolutionx, resolutiony;
};