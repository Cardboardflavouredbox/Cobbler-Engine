#pragma once

#include <model.h>

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

extern "C" {
LIB_API int GetBillBoardIndex(float angle, int lastIndex);
}

struct EntitySpawnInfo {
  int State;
  glm::vec3 position, velocityvec3 = glm::vec3({0, 0, 0});
  glm::vec2 direction;
  int teamindex;
};

struct Entity {
  unsigned int EntityIndex;

  int State;
  // float Stateanimend = 0, Stateanim = 0;

  bool invincible = false;
  float deltatimelocal = 0;
  float hp, maxhp;
  float cameraoffset;
  float movespeed = 12.5f, jumpheight = 16.f;
  glm::vec2 dir, movevec2 = glm::vec3(0);
  glm::vec3 position, velocityvec3 = glm::vec3({0, 0, 0});
  int teamindex;
  float gravity = 9.81f * 6.f;
  glm::vec3 hitbox[2];
  float hitboxradius;
  bool IsGrounded, Collided = false;
  Modeltransform* Modelthing;
  LIB_API void renderbillboard();
  LIB_API void rendermodelgroup();
  LIB_API virtual void update() = 0;
  LIB_API virtual void lateupdate() = 0;

  virtual ~Entity() {
    if (Modelthing != nullptr) delete (Modelthing);
  }
};
LIB_API extern Entity* LocalPlayer;

LIB_API extern std::map<unsigned int, Entity*> Entities;
LIB_API extern std::queue<unsigned int> EntitydeleteQueue;

LIB_API extern std::unordered_map<std::string,
                                  Entity* (*)(unsigned int, unsigned int)>
    SpawnEntities;

extern "C" {
LIB_API unsigned int EntityMapEmptyIndex();
LIB_API void EntitySpawn(std::string name, unsigned int EntityCode,
                         EntitySpawnInfo Entityinfo);
}
