#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
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

struct Particle {
  std::string Texture = "";
  glm::vec2 uv[2];
  unsigned int ParticleIndex;
  float timeleft;

  float color[4] = {1, 1, 1, 1};

  glm::vec3 position;
  glm::vec2 rect[2];

  LIB_API virtual void update() = 0;
  LIB_API virtual void lateupdate() = 0;

  virtual ~Particle() {}
};