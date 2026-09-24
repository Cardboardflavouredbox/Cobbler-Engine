#include <glm/vec2.hpp>
#include <memory>

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

enum Renderertype { Software, OpenGL1, OpenGL3, OpenGL4, Vulkan };

struct SettingsClass {
 public:
  uint16_t resolutionx = 480, resolutiony = 270;
  int fov = 90;
  int fps = 60;
  bool vsync = false;
  Renderertype graphicsmode = OpenGL1;
  glm::vec2 mousesensitivity = glm::vec2({1, 1});
};

LIB_API extern std::unique_ptr<SettingsClass> Settings;
