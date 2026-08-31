#include "extern.h"

#include <SDL3/SDL_timer.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/bitsery.h>
#include <bitsery/brief_syntax.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include "camera.h"
#include "deltaTime.h"
#include "entity.h"
#include "font.h"
#include "inputs.h"
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

std::unique_ptr<GlobalMapClass> GlobalMapStuff;

std::unique_ptr<RendererStuff> RendererGlobal;

std::unique_ptr<UIGlobalClass> UIGlobalStuff;

std::unique_ptr<GlobalNetworkClass> GlobalNetworkStuff;

template <typename S>
void serialize(S& s, ParticleSpawnInfo& o) {
  s.value4b(o.ParticleCode);
  s.container4b(o.position);
  s.text1b(o.name, 32);
}

template <typename S>
void serialize(S& s, EntitySpawnInfo& o) {
  s.value4b(o.teamindex);
  s.value4b(o.hp);
  s.text1b(o.name, 32);
  s.value4b(o.EntityCode);
  s.value4b(o.EntityIndex);
  s.container4b(o.direction);
  s.container4b(o.position);
  s.container4b(o.velocityvec3);
  s.value4b(o.State);
}

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