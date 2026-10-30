#pragma once

#include <deque>
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

extern LIB_API std::u32string cmdlines;
extern LIB_API std::u32string cmdcurrentinput;