#include "extern.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>

#include "bitserytemplates.h"
#include "camera.h"
#include "commandline.h"
#include "damageindicator.h"
#include "deltaTime.h"
#include "entity.h"
#include "font.h"
#include "inputs.h"
#include "lights.h"
#include "map.h"
#include "mapdata.h"
#include "model.h"
#include "network.h"
#include "networkextern.h"
#include "particles.h"
#include "player.h"
#include "render.h"
#include "settings.h"
#include "ui.h"

// This is where most of the extern global variables are!
float updatedeltaTime;
float renderdeltaTime;
// pointer to Global variables.
std::unique_ptr<GlobalClass> Global;
bool IsRunning;

// pointer to Settings variables.
std::unique_ptr<SettingsClass> Settings;
// pointer to Inputs of local inputs. As in, the actual keyboard inputs.
Inputs* LocalInputs;
// pointer to processed Inputs of local player. As in, move direction,
// look direction, etc.
playerinputs* P1PlayerInputs;
// LocalPlayer Entity pointer.
Entity* LocalPlayer;
LocalEntityNetData* LocalPlayerNetData;
// Entity Spawn function map. Loaded from dynamic libraries.
std::unordered_map<std::string, Entity* (*)(uint32_t, uint32_t)> SpawnEntities;
// Particle Spawn function map. Loaded from dynamic libraries.
std::unordered_map<std::string, Particle* (*)(uint32_t, uint32_t)>
    SpawnParticles;
// Player Class Update function map. Loaded from dynamic libraries.
std::unordered_map<std::string, void (*)()> PlayerClassUpdate;
// pointer to Freetype variables. (y'know for the fonts)
FreetypeClass* Freetypething;
// Map of ModelGroups. Refer to the ModelGroupClass class to see what that is.
std::unordered_map<std::string, ModelGroupClass> ModelGroupMap;
// pointer to Camera.
CameraClass* Camera;

// Entities map. Contains all the npc Entities.
std::map<uint32_t, Entity*> Entities;
// Queue that contains all the Entities to delete this frame.
std::queue<uint32_t> EntitydeleteQueue;
// Entities Pool.
std::unordered_map<std::string, std::map<uint32_t, std::queue<Entity*>>>
    EntityPool;

// Particles map. Contains all the Particles.
std::map<uint32_t, Particle*> Particles;
// Queue that contains all the Particles to delete this frame.
std::queue<uint32_t> ParticledeleteQueue;
// Particle Pooling map.
std::unordered_map<std::string, std::map<uint32_t, std::queue<Particle*>>>
    ParticlePool;

// Lights map. Contains all the Lights.
std::map<uint32_t, Light*> Lights;
// Guess what this does. It's a queue of all the Lights to be deleted.
std::queue<uint32_t> LightdeleteQueue;

std::array<float, 3> AmbientColor = {1, 1, 1};
float AmbientStrength = 0.5f;

std::unique_ptr<GlobalMapClass> GlobalMapStuff;

MapBSP* GlobalMapBSP;

std::unique_ptr<RendererStuff> RendererGlobal;

std::unique_ptr<UIGlobalClass> UIGlobalStuff;

std::unique_ptr<GlobalNetworkClass> GlobalNetworkStuff;

std::unordered_map<std::string, uint32_t> BonetoInt;
uint32_t newboneindex = 0;

std::unordered_map<std::string, uint32_t> PosetoInt;
uint32_t newposeindex = 0;

std::deque<DamageLocation> LocalDamageLocations;

std::vector<std::unordered_map<uint32_t, BoneResult>> BoneResultmapVector;

bool textinputing;

std::stringstream cmdlines;
std::string cmdcurrentinput;

uint32_t EntityMapEmptyIndex() {
  if (!Entities.contains(0)) {
    return 0;
  }
  uint32_t i = 1;
  while (i != 0) {
    if (!Entities.contains(i)) return i;
    i++;
  }
  return 0;
}

uint32_t EntitySpawn(EntitySpawnInfo Entityinfo, bool OnlineSend) {
  if (Global->IsOnline && OnlineSend && !IsServer) {
    if (!Entityinfo.SourcePlayer && Entityinfo.SourceEntityIndex == 0) {
      Entityinfo.SourcePlayer = true;
      Entityinfo.SourceEntityIndex = UserID;
    }
    std::vector<uint8_t> buffer{};

    auto writtenSize = bitsery::quickSerialization<
        bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer},
                                                            Entityinfo);

    CobblerQueueConfirmedData(0, "LocalEntity", buffer, writtenSize);
    return 0;
  }
  if (Entityinfo.SourcePlayer && Entityinfo.SourceEntityIndex == UserID) {
    Entityinfo.SourcePlayer = false;
    Entityinfo.SourceEntityIndex = 0;
  }
  uint32_t temp = EntityMapEmptyIndex();
  Entity* tempentity;
  if (EntityPool[Entityinfo.name][Entityinfo.EntityCode].empty()) {
    tempentity = SpawnEntities[Entityinfo.name](Entityinfo.EntityCode, temp);
  } else {
    tempentity = EntityPool[Entityinfo.name][Entityinfo.EntityCode].front();
    EntityPool[Entityinfo.name][Entityinfo.EntityCode].pop();

    tempentity->invincible = false;
    tempentity->weight = 1.f;
    tempentity->deltatimelocal = 0;
    tempentity->movespeed = 12.5f;
    tempentity->jumpheight = 16.f;
    tempentity->movevec2 = glm::vec3(0);
    tempentity->velocityvec3 = glm::vec3({0, 0, 0});
    tempentity->gravity = 9.81f * 6.f;
    tempentity->Collided = false;

    tempentity->setup(temp);
  }
  if (Entityinfo.hp != -1) tempentity->hp = Entityinfo.hp;
  tempentity->name = Entityinfo.name;
  tempentity->EntityCode = Entityinfo.EntityCode;

  tempentity->EntityIndex = temp;
  for (int i = 0; i < 3; i++) {
    tempentity->position[i] = Entityinfo.position[i];
    tempentity->velocityvec3[i] = Entityinfo.velocityvec3[i];
  }
  tempentity->State = Entityinfo.State;
  tempentity->teamindex = Entityinfo.teamindex;
  for (int i = 0; i < 2; i++) {
    tempentity->dir[i] = Entityinfo.direction[i];
  }

  if (Entityinfo.SourceExists) {
    if (Entityinfo.SourcePlayer) {
      tempentity->SourceEntity =
          GlobalNetworkStuff->PlayerNetStuff[Entityinfo.SourceEntityIndex]
              .PlayerEntity;
    } else {
      tempentity->SourceEntity = Entities[Entityinfo.SourceEntityIndex];
    }
  } else {
    tempentity->SourceEntity = NULL;
  }

  Entities[temp] = tempentity;
  return temp;
}

void DamageIndicatorPush(EntityDamageInfo damageinfo) {
  DamageLocation damageloc;

  if (damageinfo.FromPlayer && damageinfo.SourceEntityIndex == UserID) {
    damageinfo.FromPlayer = false;
    damageinfo.SourceEntityIndex = 0;
  }
  damageloc.lifestart = damageinfo.damage / 4.f;
  damageloc.lifetime = damageloc.lifestart;

  if (damageinfo.FromPlayer) {
    for (int i = 0; i < 3; i++) {
      damageloc.position[i] =
          GlobalNetworkStuff->PlayerNetStuff[damageinfo.SourceEntityIndex]
              .PlayerEntity->position[i];
    }
    damageloc.position[2] +=
        GlobalNetworkStuff->PlayerNetStuff[damageinfo.SourceEntityIndex]
            .PlayerEntity->cameraoffset;
  } else {
    for (int i = 0; i < 3; i++) {
      damageloc.position[i] =
          Entities[damageinfo.SourceEntityIndex]->position[i];
    }
    damageloc.position[2] +=
        Entities[damageinfo.SourceEntityIndex]->cameraoffset;
  }

  if (damageinfo.TargetEntityIndex == 0 && !damageinfo.TargetIsPlayer) {
    LocalDamageLocations.push_back(damageloc);
  } else if (damageinfo.TargetIsPlayer) {
    std::vector<uint8_t> buffer{};
    auto writtenSize = bitsery::quickSerialization<
        bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer},
                                                            damageloc);
    CobblerQueueConfirmedData(damageinfo.TargetEntityIndex, "DamageLocation",
                              buffer, writtenSize);
  }
}

void DamageEntity(EntityDamageInfo damageinfo, bool FromLocalPlayer) {
  if (FromLocalPlayer || !Global->IsOnline || IsServer) {
    if (!Global->IsOnline || IsServer) {
      if (damageinfo.TargetIsPlayer) {
        if (!GlobalNetworkStuff->PlayerNetStuff[damageinfo.TargetEntityIndex]
                 .PlayerEntity->invincible) {
          GlobalNetworkStuff->PlayerNetStuff[damageinfo.TargetEntityIndex]
              .PlayerEntity->hp -= damageinfo.damage;

          if (damageinfo.push == -1) {
            damageinfo.push = damageinfo.damage /= 8;
          }
          GlobalNetworkStuff->PlayerNetStuff[damageinfo.TargetEntityIndex]
              .PlayerEntity->velocityvec3 +=
              glm::vec3(damageinfo.pushdir[0], damageinfo.pushdir[1],
                        damageinfo.pushdir[2]) *
              damageinfo.push;
        }
        DamageIndicatorPush(damageinfo);
      } else {
        if (!Entities[damageinfo.TargetEntityIndex]->invincible) {
          Entities[damageinfo.TargetEntityIndex]->hp -= damageinfo.damage;

          if (damageinfo.push == -1) {
            damageinfo.push = damageinfo.damage /= 8;
          }
          Entities[damageinfo.TargetEntityIndex]->velocityvec3 +=
              glm::vec3(damageinfo.pushdir[0], damageinfo.pushdir[1],
                        damageinfo.pushdir[2]) *
              damageinfo.push;
        }
        if (damageinfo.TargetEntityIndex == 0) {
          DamageIndicatorPush(damageinfo);
        }
      }
    } else {
      if (!damageinfo.FromPlayer && damageinfo.SourceEntityIndex == 0) {
        damageinfo.FromPlayer = true;
        damageinfo.SourceEntityIndex = UserID;
      }
      std::vector<uint8_t> buffer{};

      auto writtenSize = bitsery::quickSerialization<
          bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer},
                                                              damageinfo);
      CobblerQueueConfirmedData(0, "DamageEntity", buffer, writtenSize);
    }
  }
}

uint32_t ParticleMapEmptyIndex() {
  if (!Particles.contains(0)) {
    return 0;
  }
  uint32_t i = 1;
  while (i != 0) {
    if (!Particles.contains(i)) return i;
    i++;
  }
  return 0;
}

uint32_t LightMapEmptyIndex() {
  if (!Lights.contains(0)) {
    return 0;
  }
  uint32_t i = 1;
  while (i != 0) {
    if (!Lights.contains(i)) return i;
    i++;
  }
  return 0;
}

glm::vec3 GetRespawnPoint(int teamindex) {
  return GlobalMapStuff
      ->SpawnPoints[(teamindex % int(GlobalMapStuff->SpawnPoints.size()))];
}

void ParticleSpawn(ParticleSpawnInfo Particleinfo, bool OnlineSend) {
  if (Global->IsOnline && OnlineSend) {  // online code stuff
    if (!Particleinfo.SourcePlayer && Particleinfo.SourceEntityIndex == 0) {
      Particleinfo.SourcePlayer = true;
      Particleinfo.SourceEntityIndex = UserID;
    }
    std::vector<uint8_t> buffer{};

    auto writtenSize = bitsery::quickSerialization<
        bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer},
                                                            Particleinfo);

    CobblerQueueData("ParticleSpawn", buffer, writtenSize);
  }

  if (Particleinfo.SourcePlayer && Particleinfo.SourceEntityIndex == UserID) {
    Particleinfo.SourcePlayer = false;
    Particleinfo.SourceEntityIndex = 0;
  }

  uint32_t temp = ParticleMapEmptyIndex();
  Particle* tempparticle;
  if (ParticlePool[Particleinfo.name][Particleinfo.ParticleCode].empty()) {
    tempparticle =
        SpawnParticles[Particleinfo.name](Particleinfo.ParticleCode, temp);
  } else {
    tempparticle =
        ParticlePool[Particleinfo.name][Particleinfo.ParticleCode].front();
    ParticlePool[Particleinfo.name][Particleinfo.ParticleCode].pop();
    tempparticle->setup(temp);
  }

  if (Particleinfo.SourceExists) {
    if (Particleinfo.SourcePlayer) {
      tempparticle->SourceEntity =
          GlobalNetworkStuff->PlayerNetStuff[Particleinfo.SourceEntityIndex]
              .PlayerEntity;
    } else {
      tempparticle->SourceEntity = Entities[Particleinfo.SourceEntityIndex];
    }
  } else {
    tempparticle->SourceEntity = NULL;
  }

  tempparticle->name = Particleinfo.name;
  tempparticle->ParticleCode = Particleinfo.ParticleCode;
  for (int i = 0; i < 3; i++) {
    tempparticle->position[i] = Particleinfo.position[i];
    tempparticle->direction[i] = Particleinfo.direction[i];
  }
  Particles[temp] = tempparticle;
}