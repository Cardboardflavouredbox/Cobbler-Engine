#pragma once

#include <SDL3/SDL_render.h>

#include <unordered_map>

struct GlobalClass {
 public:
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Surface* render_target;

  bool IsRunning;
  float deltaTime;

  std::unordered_map<std::string, SDL_Surface*> texturemap;
};

struct Inputs {
 public:
  unsigned char W = 0, A = 0, S = 0, D = 0;
  Vector2 Mouse;
};

struct SettingsClass {
 public:
  uint16_t resolutionx, resolutiony;
  int fov = 90;
};