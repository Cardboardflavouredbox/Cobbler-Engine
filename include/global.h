#pragma once

#include <SDL3/SDL_render.h>
#include <ft2build.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

#include "map.h"
#include "ui.h"
#include FT_FREETYPE_H

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

const double PI =
    3.1415926535897932384626433832795028841971693993751058209749445923078164062;

struct CustomGlyphthing {
  unsigned char* pixels;
  GLuint GLTexture;
  int width, height, pitch, offsetx, offsety, advancex, advancey;
};

extern "C" {
LIB_API CustomGlyphthing CreateGlyph(FT_GlyphSlot glyph);
}
struct GlobalClass {
 public:
  std::string GameName = "CobblerGame";

  SDL_Window* window;

  struct SoftwareRenderer {
    SDL_Renderer* renderer;
    SDL_Surface* render_target;
    SDL_Palette* palette;
    std::unordered_map<std::string, SDL_Surface*> textures;
    unsigned char* pixels;
    int pitch;
    std::vector<unsigned short> pixelsdepth;
  };

  SoftwareRenderer* SRstuff;

  struct OpenGLRenderer {
    SDL_GLContext GLContext;
    std::unordered_map<std::string, GLuint> textures;
  };
  OpenGLRenderer* GLstuff;

  bool IsRunning;
  bool pause = false, isopeningfile = false;
  float deltaTime;
  std::string skybox;
  bool IsEditor;

  int windowx = 320, windowy = 200;

  std::vector<glm::vec3> Points;
  std::vector<Mapface> mapfaces;
  std::vector<Entity*> Entities;

  FT_Library FTlibrary;
  FT_Face FTface;
  std::unordered_map<uint32_t, CustomGlyphthing> Glyphmap;

  std::unordered_map<std::string, std::vector<UIthing*>> UImap;
  std::vector<std::string> UIlist = {"default"};
  glm::mat4 perspectivematrix;
};

struct EditorClass {
  glm::vec2 pos;
  float zoom = 1;
  int currentlyselectedpoint = -1, currentlyselectedface = -1;
};

struct ZipData {
  std::string startlevel, fontname;
  std::vector<std::string> texturenames;
  std::vector<std::string> stagenames;
};

struct Mapdata {
  struct Entitydata {
    std::string name;
    glm::vec3 pos;
  };
  std::vector<glm::vec3> Points;
  std::vector<Mapface> mapfaces;
  std::vector<Entitydata> Entities;
  std::string skybox;
};

struct Inputs {
 public:
  unsigned char W = 0, A = 0, S = 0, D = 0, ESC = 0, LCTRL = 0, Shift = 0,
                Space = 0, leftclick = 0, rightclick = 0, numkeys[10] = {},
                E = 0, F = 0, Enter = 0;
  glm::vec2 MouseDelta, MousePos, MouseScroll;
};

struct SettingsClass {
 public:
  uint16_t resolutionx = 480, resolutiony = 270;
  int fov = 90;
  int fps = 60;
  bool vsync = false;
  int graphicsmode = 0;  // 0 = software, 1 = opengl
  bool autorun = false;
  glm::vec2 mousesensitivity = glm::vec2({1, 1});
};