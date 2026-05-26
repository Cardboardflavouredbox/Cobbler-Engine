#include <string>
#include <vector>

#ifdef _WIN32
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT
#endif

LIB_EXPORT void savemap();

LIB_EXPORT bool initargs(std::vector<std::string> args);

LIB_EXPORT bool init(bool hidemouse);

LIB_EXPORT void SaveSettings();

LIB_EXPORT void LoadSettings();

LIB_EXPORT void quit();