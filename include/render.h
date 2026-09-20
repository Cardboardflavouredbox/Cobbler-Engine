#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_vulkan.h>
#include <glad/gl.h>

#include <array>
#include <filesystem>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vulkan/vulkan_raii.hpp>

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

  std::array<float, 4> viewportdata;

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
    struct GLObject {
      GLuint texture;
      GLuint VBOthing, VAOthing;
      uint32_t size = 0;
      GLuint shader;
    };
    GLuint MapGLlist;
    std::vector<GLObject> GlMapObjects;

    std::unordered_map<std::string, GLObject> GLModels;
    GLObject GLParticleBase;
    GLObject GLSkybox;

    std::unordered_map<std::string, GLuint> shaders;

    GLuint emptyVAO;

    GLuint FBO, RBO, FBTexture;
    GLObject GLFrameBufferthing;
  };
  OpenGLRenderer* GLstuff;

  struct VulkanRenderer {
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::PhysicalDevice physicalDevice = nullptr;
  };
  VulkanRenderer* Vulkanstuff;
};

LIB_API extern std::unique_ptr<RendererStuff> RendererGlobal;

extern "C" {
LIB_API void renderModelGroup(Modeltransform* modeltrans,
                              std::string modelgroupname, bool isUI,
                              float deltatime);

LIB_API void DrawLine(unsigned char color, glm::vec3 rawvectors[]);

LIB_API void DrawCircle(unsigned char color, glm::vec3 rawpoint, int radius);

LIB_API void DrawTri(std::string texture, glm::vec3 rawvectors[],
                     glm::vec2 UVs[]);

LIB_API GLuint LoadShaders(std::vector<std::filesystem::path> file_paths);
}
