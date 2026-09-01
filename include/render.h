#pragma once

#include <SDL3/SDL_render.h>
#include <glad/glad.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <memory>

#include "model.h"

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

struct RendererStuff {
  SDL_Window* window;

  int windowx = 320, windowy = 200;

  float windowscale = 1.f;

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
    GLuint MapGLlist;
  };
  OpenGLRenderer* GLstuff;
};

LIB_API extern std::unique_ptr<RendererStuff> RendererGlobal;

extern "C" {
LIB_API void renderModelGroup(Modeltransform* modeltrans,
                              ModelGroupClass* modelgroup, bool isUI,
                              float deltatime);

LIB_API void DrawLine(unsigned char color, glm::vec3 rawvectors[]);

LIB_API void DrawCircle(unsigned char color, glm::vec3 rawpoint, int radius);

LIB_API void DrawTri(std::string texture, glm::vec3 rawvectors[],
                     glm::vec2 UVs[], int xloop, int yloop);
}
