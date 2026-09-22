#include "extern.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>

#include "bitserytemplates.h"
#include "camera.h"
#include "deltaTime.h"
#include "entity.h"
#include "font.h"
#include "inputs.h"
#include "lights.h"
#include "map.h"
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
// pointer to Settings variables.
std::unique_ptr<SettingsClass> Settings;
// pointer to Inputs of local inputs. As in, the actual keyboard inputs.
Inputs* LocalInputs;
// pointer to processed Inputs of local player. As in, move direction,
// look direction, etc.
playerinputs* P1PlayerInputs;
// LocalPlayer Entity pointer.
Entity* LocalPlayer;
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

// Particles map. Contains all the Particles.
std::map<uint32_t, Particle*> Particles;
// Queue that contains all the Particles to delete this frame.
std::queue<uint32_t> ParticledeleteQueue;

// Lights map. Contains all the Lights.
std::map<uint32_t, Light*> Lights;
// Guess what this does. It's a queue of all the Lights to be deleted.
std::queue<uint32_t> LightdeleteQueue;

std::array<float, 3> AmbientColor = {1, 1, 1};
float AmbientStrength = 0.5f;

std::unique_ptr<GlobalMapClass> GlobalMapStuff;

std::unique_ptr<RendererStuff> RendererGlobal;

std::unique_ptr<UIGlobalClass> UIGlobalStuff;

std::unique_ptr<GlobalNetworkClass> GlobalNetworkStuff;

std::unordered_map<std::string, uint32_t> BonetoInt;
uint32_t newboneindex = 0;

std::unordered_map<std::string, uint32_t> PosetoInt;
uint32_t newposeindex = 0;

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
    std::vector<uint8_t> buffer{};

    auto writtenSize = bitsery::quickSerialization<
        bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer},
                                                            Entityinfo);

    CobblerQueueData("LocalEntity", buffer, writtenSize);
    return 0;
  }
  uint32_t temp = EntityMapEmptyIndex();
  Entity* tempentity =
      SpawnEntities[Entityinfo.name](Entityinfo.EntityCode, temp);

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
  Entities[temp] = tempentity;
  return temp;
}

void DamageEntity(EntityDamageInfo damageinfo, bool FromLocalPlayer) {
  if (FromLocalPlayer || !Global->IsOnline || IsServer) {
    SDL_Log("DamageEntity Index: %llu Damage: %f", damageinfo.EntityIndex,
            damageinfo.damage);
    if (IsServer) {
      if (damageinfo.IsPlayer) {
        if (!GlobalNetworkStuff->PlayerNetStuff[damageinfo.EntityIndex]
                 .PlayerEntity->invincible)
          GlobalNetworkStuff->PlayerNetStuff[damageinfo.EntityIndex]
              .PlayerEntity->hp -= damageinfo.damage;
      } else {
        if (!Entities[damageinfo.EntityIndex]->invincible)
          Entities[damageinfo.EntityIndex]->hp -= damageinfo.damage;
      }
    } else {
      std::vector<uint8_t> buffer{};

      auto writtenSize = bitsery::quickSerialization<
          bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer},
                                                              damageinfo);
      CobblerQueueData("DamageEntity", buffer, writtenSize);
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

void ParticleSpawn(ParticleSpawnInfo Particleinfo, bool OnlineSend) {
  if (Global->IsOnline && OnlineSend) {  // online code stuff
    std::vector<uint8_t> buffer{};

    auto writtenSize = bitsery::quickSerialization<
        bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer},
                                                            Particleinfo);

    CobblerQueueData("ParticleSpawn", buffer, writtenSize);
  }
  uint32_t temp = ParticleMapEmptyIndex();
  Particle* tempparticle =
      SpawnParticles[Particleinfo.name](Particleinfo.ParticleCode, temp);
  for (int i = 0; i < 3; i++) {
    tempparticle->position[i] = Particleinfo.position[i];
  }
  Particles[temp] = tempparticle;
}