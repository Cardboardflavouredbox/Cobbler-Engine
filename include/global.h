#pragma once

#include <SDL3/SDL_render.h>

#include <vector>

struct GlobalClass {
 public:
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Surface* render_target;

  bool IsRunning;
  float deltaTime;

  std::vector<SDL_Surface*> textures;
};

struct ZipData {
  std::vector<std::string> texturenames;
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