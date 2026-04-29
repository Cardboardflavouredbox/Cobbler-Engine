#pragma once

#include <SDL3/SDL_render.h>
#include <ft2build.h>
#include <glad/glad.h>

#include <deque>
#include <string>
#include <unordered_map>

#include "map.h"
#include "ui.h"
#include FT_FREETYPE_H

const double PI =
    3.1415926535897932384626433832795028841971693993751058209749445923078164062;

struct CustomGlyphthing {
  unsigned char* pixels;
  int width, height, pitch, offsetx, offsety, advancex, advancey;
};

CustomGlyphthing CreateGlyph(FT_GlyphSlot glyph);

struct GlobalClass {
 public:
  SDL_Window* window;

  struct SoftwareRenderer {
    SDL_Renderer* renderer;
    SDL_Surface* render_target;
    SDL_Palette* palette;
    std::vector<SDL_Surface*> textures;
    unsigned char* pixels;
    int pitch;
    std::vector<unsigned short> pixelsdepth;
    std::vector<unsigned char> pixelstransparency;
  };

  SoftwareRenderer* SRstuff;

  struct OpenGLRenderer {
    SDL_GLContext GLContext;
    std::vector<GLuint> textures;
  };
  OpenGLRenderer* GLstuff;

  bool IsRunning;
  bool pause = false, isopeningfile = false;
  float deltaTime;
  int skybox;
  int rendermode = 0;  // 0==default, 1==wireframe and points
  bool IsEditor;

  int windowx = 320, windowy = 200;

  std::deque<glm::vec3> Points;
  std::deque<Mapface> mapfaces;
  std::deque<Entity*> Entities;

  FT_Library FTlibrary;
  FT_Face FTface;
  std::unordered_map<uint32_t, CustomGlyphthing> Glyphmap;

  std::unordered_map<int, std::deque<UIthing*>> UImap;
  int UIindex = 0;
};

struct EditorClass {
  glm::vec2 pos;
  float zoom = 1;
};

struct ZipData {
  std::string startlevel;
  std::deque<std::string> texturenames;
  std::deque<std::string> stagenames;
};

struct Mapdata {
  std::deque<glm::vec3> Points;
  std::deque<Mapface> mapfaces;
  int skybox;
};

struct Inputs {
 public:
  unsigned char W = 0, A = 0, S = 0, D = 0, ESC = 0, LCTRL = 0, Shift = 0,
                Space = 0, leftclick = 0, rightclick = 0, numkeys[10] = {},
                E = 0, F = 0;
  glm::vec2 MouseDelta, MousePos, MouseScroll;
};

struct SettingsClass {
 public:
  uint16_t resolutionx = 320, resolutiony = 200;
  int fov = 90;
  int fps = 60;
  bool vsync = true;
  int graphicsmode = 0;  // 0 = software, 1 = opengl
};