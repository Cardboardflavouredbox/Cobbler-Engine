#include "glm/vec2.hpp"
#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

struct Inputs {
 public:
  unsigned char Keys[512] = {}, leftclick = 0, rightclick = 0;
  glm::vec2 MouseDelta, MousePos, MouseScroll;
};
LIB_API extern Inputs* LocalInputs;