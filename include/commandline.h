#pragma once

#include <deque>
#include <sstream>
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

extern LIB_API bool textinputing;
extern LIB_API std::stringstream cmdlines;
extern LIB_API std::string cmdcurrentinput;

extern "C" {
LIB_API void pushcmd();
}