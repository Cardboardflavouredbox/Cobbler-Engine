#pragma once

#include <model.h>

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

extern "C" {
LIB_API int GetBillBoardIndex(float angle, int lastIndex);
}
struct Entity {
  int State;
  // float Stateanimend = 0, Stateanim = 0;

  float deltatimelocal = 0;
  float hp, maxhp;
  float movespeed = 17.5f, jumpheight = 24.f;
  glm::vec2 dir, movevec2 = glm::vec3(0);
  glm::vec3 position, velocityvec3 = glm::vec3({0, 0, 0});
  int teamindex;
  float gravity = 9.81f * 8.f;
  glm::vec3 hitbox[2];
  float hitboxradius;
  bool IsGrounded;
  Modeltransform* Modelthing;
  LIB_API void renderbillboard();
  LIB_API void rendermodelgroup();
  LIB_API virtual void update() = 0;
  LIB_API virtual void lateupdate() = 0;

  virtual ~Entity() {
    if (Modelthing != nullptr) delete (Modelthing);
  }
};

struct CameraEntity : Entity {
  void update() {}
  void lateupdate() {}
  ~CameraEntity() {}
};