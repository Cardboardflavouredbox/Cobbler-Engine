#pragma once

#include "network.h"

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
LIB_API void ServerSetup(uint32_t ServerPort);
LIB_API void ClientConnect(const std::string& ServerIP, uint32_t ServerPort);
}