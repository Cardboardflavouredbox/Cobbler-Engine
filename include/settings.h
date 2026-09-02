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

struct SettingsClass {
 public:
  uint16_t resolutionx = 480, resolutiony = 270;
  int fov = 90;
  int fps = 60;
  bool vsync = false;
  uint8_t graphicsmode = 0;  // 0 = software, 1 = opengl 2 = vulkan
  bool autorun = false;
  glm::vec2 mousesensitivity = glm::vec2({1, 1});
};

LIB_API extern std::unique_ptr<SettingsClass> Settings;
