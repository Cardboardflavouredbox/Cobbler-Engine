#ifdef _WIN32
  #ifdef DLLEXPORT
    #define LIB_API __declspec(dllexport)
  #else
    #define LIB_API __declspec(dllimport)
  #endif
#else
#define LIB_API
#endif

extern "C" {
  LIB_API void input();
  LIB_API void update();
  LIB_API void render();
  LIB_API void events();
}