#pragma once

#include <SDL3/SDL_render.h>

#include <deque>
#include <string>

#include "map.h"

struct GlobalClass {
 public:
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Surface* render_target;

  bool IsRunning;
  bool pause;
  float deltaTime;

  std::vector<SDL_Surface*> textures;
  std::deque<Vector3> Points;
  std::deque<Mapface> mapfaces;
  std::deque<Entity*> Entities;
};

struct ZipData {
  std::string startlevel;
  std::deque<std::string> texturenames;
  std::deque<std::string> stagenames;
};

struct Mapdata {
  std::deque<Vector3> Points;
  std::deque<Mapface> mapfaces;
};

struct Inputs {
 public:
  unsigned char W = 0, A = 0, S = 0, D = 0, ESC = 0, Shift = 0, Space = 0;
  Vector2 Mouse;
};

struct SettingsClass {
 public:
  uint16_t resolutionx, resolutiony;
  int fov = 90;
};