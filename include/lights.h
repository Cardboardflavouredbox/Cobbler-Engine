#pragma once

#include <deltaTime.h>

#include <glm/vec3.hpp>
#include <map>
#include <queue>

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

LIB_API extern std::queue<uint32_t> LightdeleteQueue;

struct Light {
  uint32_t Lightindex;
  float timeleft;

  float color[3] = {1, 1, 1};

  glm::vec3 position;

  LIB_API virtual void update() = 0;
  LIB_API virtual void lateupdate() = 0;

  virtual ~Light() {}
};

struct StaticLight : Light {
  StaticLight(uint32_t Index) { Lightindex = Index; }
  void update() {}
  void lateupdate() {}
};

struct TempLight : Light {
  TempLight(uint32_t Index, float length) {
    Lightindex = Index;
    timeleft = length;
  }
  void update() {
    timeleft -= updatedeltaTime;
    if (timeleft <= 0) {
      LightdeleteQueue.push(Lightindex);
    }
  }
  void lateupdate() {}
};

LIB_API extern std::map<uint32_t, Light*> Lights;

extern "C" {
LIB_API uint32_t LightMapEmptyIndex();
}