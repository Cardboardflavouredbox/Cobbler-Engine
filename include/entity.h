#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>

struct Entity {
  float hp;
  glm::vec2 dir;
  glm::vec3 position, movevec3 = glm::vec3({0, 0, 0}),
                      velocityvec3 = glm::vec3({0, 0, 0});
  float gravity = 9.81f * 8.f;
  float jumpheight = 24, walkspeed = 10.f, runspeed = 1.75f;
  glm::vec3 hitbox[2];
  bool IsGrounded;
  struct Billboard {
    std::string sprite;
    glm::vec2 size, uv[2];
  };
  Billboard* billboardthing;
  void render();

  ~Entity() {
    if (billboardthing != nullptr) delete (billboardthing);
  }
};