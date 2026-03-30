#pragma once

#include <SDL3/SDL_render.h>
#include <ft2build.h>

#include <deque>
#include <string>
#include <unordered_map>

#include "map.h"
#include "ui.h"
#include FT_FREETYPE_H

struct CustomGlyphthing {
  unsigned char* pixels;
  int width, height, pitch, offsetx, offsety, advancex, advancey;
};

CustomGlyphthing CreateGlyph(FT_GlyphSlot glyph);

struct GlobalClass {
 public:
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Surface* render_target;
  SDL_Palette* palette;

  bool IsRunning;
  bool pause = false, isopeningfile = false;
  float deltaTime;
  int skybox;
  int rendermode = 0;  // 0==default, 1==wireframe and points
  int editorselectedPoint = 0;
  int editorselectedFace = 0;
  int editordraggingaxis = -1;

  int windowx = 320, windowy = 200;

  std::vector<SDL_Surface*> textures;
  std::deque<Vector3> Points;
  std::deque<Mapface> mapfaces;
  std::deque<Entity*> Entities;

  FT_Library FTlibrary;
  FT_Face FTface;
  std::unordered_map<uint32_t, CustomGlyphthing> Glyphmap;

  std::unordered_map<int, std::deque<UIthing*>> UImap;
};

struct ZipData {
  std::string startlevel;
  std::deque<std::string> texturenames;
  std::deque<std::string> stagenames;
};

struct Mapdata {
  std::deque<Vector3> Points;
  std::deque<Mapface> mapfaces;
  int skybox;
};

struct Inputs {
 public:
  unsigned char W = 0, A = 0, S = 0, D = 0, ESC = 0, LCTRL = 0, Shift = 0,
                Space = 0, leftclick = 0, rightclick = 0, numkeys[10] = {},
                E = 0, F = 0;
  Vector2 MouseDelta, MousePos, MouseScroll;
};

struct SettingsClass {
 public:
  uint16_t resolutionx = 640, resolutiony = 400;
  int fov = 90;
  int fps = 60;
  bool vsync = true;
};