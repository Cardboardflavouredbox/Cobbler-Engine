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
  LIB_API void componentsupdate();
  LIB_API void componentsupdatelate();
}