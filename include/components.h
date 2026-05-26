#ifdef _WIN32
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT
#endif

LIB_EXPORT void componentsupdate();
LIB_EXPORT void componentsupdatelate();