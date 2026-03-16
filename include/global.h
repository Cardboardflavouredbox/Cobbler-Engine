#pragma once

#include <SDL3/SDL_render.h>

#include <string>
#include <vector>

#include "map.h"

struct GlobalClass {
 public:
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Surface* render_target;

  bool IsRunning;
  float deltaTime;

  std::vector<SDL_Surface*> textures;
  std::vector<Vector3> Points;
  std::vector<Mapface> mapfaces;
};

struct ZipData {
  std::string startlevel;
  std::vector<std::string> texturenames;
  std::vector<std::string> stagenames;
};

struct Mapdata {
  std::vector<Vector3> Points;
  std::vector<Mapface> mapfaces;
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