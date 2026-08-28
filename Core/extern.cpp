#include "extern.h"

#include <SDL3/SDL_timer.h>

#include "camera.h"
#include "deltaTime.h"
#include "entity.h"
#include "font.h"
#include "inputs.h"
#include "map.h"
#include "model.h"
#include "networkextern.h"
#include "particles.h"
#include "player.h"
#include "render.h"
#include "settings.h"
#include "ui.h"

// This is where most of the extern global variables are!
float deltaTime;

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
// deltatime calculation variable
uint64_t lastTime;
// deltatime calculation variable
uint64_t currentTime = SDL_GetPerformanceCounter();
// Entity Spawn function map. Loaded from dynamic libraries.
std::unordered_map<std::string, Entity* (*)(unsigned int, unsigned int)>
    SpawnEntities;
// Particle Spawn function map. Loaded from dynamic libraries.
std::unordered_map<std::string, Particle* (*)(unsigned int, unsigned int)>
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
std::map<unsigned int, Entity*> Entities;
// Queue that contains all the Entities to delete this frame.
std::queue<unsigned int> EntitydeleteQueue;

// Particles map. Contains all the Particles.
std::map<unsigned int, Particle*> Particles;
// Queue that contains all the Particles to delete this frame.
std::queue<unsigned int> ParticledeleteQueue;

std::unique_ptr<GlobalMapClass> GlobalMapStuff;

std::unique_ptr<RendererStuff> RendererGlobal;

std::unique_ptr<UIGlobalClass> UIGlobalStuff;

std::unique_ptr<GlobalNetworkClass> GlobalNetworkStuff;

unsigned int EntityMapEmptyIndex() {
  if (!Entities.contains(0)) {
    return 0;
  }
  unsigned int i = 1;
  while (i != 0) {
    if (!Entities.contains(i)) return i;
    i++;
  }
  return 0;
}

void EntitySpawn(std::string name, unsigned int EntityCode,
                 EntitySpawnInfo Entityinfo) {
  if (!Global->IsOnline || GlobalNetworkStuff->IsServer) {
    unsigned int temp = EntityMapEmptyIndex();
    Entity* tempentity = SpawnEntities[name](EntityCode, temp);
    tempentity->EntityIndex = temp;
    tempentity->position = Entityinfo.position;
    tempentity->State = Entityinfo.State;
    tempentity->teamindex = Entityinfo.teamindex;
    tempentity->velocityvec3 = Entityinfo.velocityvec3;
    tempentity->dir = Entityinfo.direction;
    Entities[temp] = tempentity;
  }
}

unsigned int ParticleMapEmptyIndex() {
  if (!Particles.contains(0)) {
    return 0;
  }
  unsigned int i = 1;
  while (i != 0) {
    if (!Particles.contains(i)) return i;
    i++;
  }
  return 0;
}

void ParticleSpawn(std::string name, unsigned int ParticleCode,
                   glm::vec3 position) {
  if (Global->IsOnline) {  // online code stuff
  }
  unsigned int temp = ParticleMapEmptyIndex();
  Particle* tempparticle = SpawnParticles[name](ParticleCode, temp);
  tempparticle->position = position;
  Particles[temp] = tempparticle;
}