#ifdef _WIN32
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT
#endif

LIB_EXPORT void input();
LIB_EXPORT void update();
LIB_EXPORT void render();
LIB_EXPORT void events();