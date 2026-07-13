#pragma once

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

struct PostField {
  std::string username, websiteaddr = "127.0.0.1:80";
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

struct CobblerNetData {
  std::string name, IP;
  std::vector<Uint8> buffer;
  Uint16 PORT;
  Uint64 ID;
};

LIB_API extern PostField* curlpostfield;
LIB_API extern std::string curlloginstring;
LIB_API extern bool IsServer;

extern "C" {
LIB_API bool CobblerInitNet();
LIB_API void CobblerAddIP(std::string IP, unsigned int Port, Uint64 ID);
LIB_API bool CobblerCheckHasIP(std::string IP, unsigned int Port);
LIB_API bool CobblerSetSocket(unsigned int port);
LIB_API void CobblerQuitNet();
LIB_API bool CobblerSendCurlData();
LIB_API bool CobblerCurlLogin();
LIB_API bool CobblerQueueData(const char* name, std::vector<Uint8> buf);
LIB_API bool CobblerSendNet();
LIB_API std::vector<CobblerNetData>* CobblerRecvNet();
}