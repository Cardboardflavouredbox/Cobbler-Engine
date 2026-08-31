#pragma once

#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <map>
#include <queue>
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

struct ParticleSpawnInfo {
  std::string name;
  uint32_t ParticleCode;
  std::array<float, 3> position;
};

struct Particle {
  std::string Texture = "";
  glm::vec2 uv[2];
  uint32_t ParticleIndex;
  float timeleft;

  float color[4] = {1, 1, 1, 1};

  glm::vec3 position;
  glm::vec2 rect[2];

  LIB_API virtual void update() = 0;
  LIB_API virtual void lateupdate() = 0;

  virtual ~Particle() {}
};

LIB_API extern std::map<uint32_t, Particle*> Particles;
LIB_API extern std::queue<uint32_t> ParticledeleteQueue;

LIB_API extern std::unordered_map<std::string,
                                  Particle* (*)(uint32_t, uint32_t)>
    SpawnParticles;

extern "C" {
LIB_API uint32_t ParticleMapEmptyIndex();
LIB_API void ParticleSpawn(ParticleSpawnInfo Particleinfo, bool OnlineSend);
}