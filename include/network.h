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

struct PostField {
  std::string username;
  bool hasdata = false;
  int Kills = 0;

  void reset() {
    Kills = 0;
    hasdata = false;
  }

  std::string ToField() {
    std::string result = "username=" + username + "&";
    if (Kills > 0) {
      result += "Kills=" + std::to_string(Kills) + "&";
    }

    if (!result.empty()) result.pop_back();
    return result;
  }
};

LIB_API extern PostField* curlpostfield;
LIB_API extern std::string curlloginstring;

extern "C" {
LIB_API bool CobblerInitNet();
LIB_API void CobblerQuitNet();
LIB_API bool CobblerSendCurlData();
LIB_API bool CobblerCurlLogin();
}