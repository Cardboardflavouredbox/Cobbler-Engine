#pragma once

#include <string>

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

LIB_API extern std::string curlpostfields;

extern "C" {
LIB_API bool CobblerInitNet();
LIB_API void CobblerQuitNet();
LIB_API bool CobblerSendCurlData();
LIB_API bool CobblerCurlLogin();
}