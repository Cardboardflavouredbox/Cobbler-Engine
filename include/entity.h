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
  float hp;
  glm::vec2 dir;
  glm::vec3 position, movevec3 = glm::vec3({0, 0, 0}),
                      velocityvec3 = glm::vec3({0, 0, 0});
  int teamindex;
  float gravity = 9.81f * 8.f;
  float jumpheight = 24, walkspeed = 10.f, runspeed = 1.75f;
  glm::vec3 hitbox[2];
  float hitboxradius;
  bool IsGrounded;
  struct Billboard {
    std::string sprite;
    glm::vec2 size, uv[2];
    float offset;
  };
  Billboard* billboardthing;
  Modeltransform* Modelthing;
  LIB_API void renderbillboard();
  LIB_API void rendermodelgroup();
  LIB_API virtual void update() = 0;
  LIB_API virtual void lateupdate() = 0;

  virtual ~Entity() {
    if (billboardthing != nullptr) delete (billboardthing);
    if (Modelthing != nullptr) delete (Modelthing);
  }
};

struct CameraEntity : Entity {
  void update() {}
  void lateupdate() {}
  ~CameraEntity() {}
};