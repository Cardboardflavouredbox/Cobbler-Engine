#include <string>
#include <vector>

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
  LIB_API void savemap();
  
  LIB_API bool initargs(std::vector<std::string> args);
  
  LIB_API bool init(bool hidemouse);
  
  LIB_API void SaveSettings();
  
  LIB_API void LoadSettings();
  
  LIB_API void quit();
}