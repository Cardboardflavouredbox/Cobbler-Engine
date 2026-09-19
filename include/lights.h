#pragma once

#include <deltaTime.h>

#include <array>
#include <cmath>
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
  float starttime, timeleft;

  float startspecularStrength = 0.5f, specularStrength = 0.5f;
  float startdiffusionStrength = 1.f, diffusionStrength = 1.f;

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
    starttime = length;
    timeleft = length;
  }
  void update() {
    timeleft -= updatedeltaTime;
    if (timeleft <= 0) {
      LightdeleteQueue.push(Lightindex);
    }
  }
  void lateupdate() {
    float t = 1 - timeleft / starttime;
    specularStrength = std::lerp(startspecularStrength, 0, t);
    diffusionStrength = std::lerp(startdiffusionStrength, 0, t);
  }
};

LIB_API extern std::map<uint32_t, Light*> Lights;
LIB_API extern std::array<float, 3> AmbientColor;
LIB_API extern float AmbientStrength;

extern "C" {
LIB_API uint32_t LightMapEmptyIndex();
}