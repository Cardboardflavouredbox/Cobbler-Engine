#include "extern.h"

#include <SDL3/SDL_timer.h>

#include "camera.h"
#include "deltaTime.h"
#include "font.h"
#include "inputs.h"
#include "map.h"
#include "model.h"
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
std::unordered_map<std::string, Entity* (*)(unsigned int)> SpawnEntities;
// Player Class Update function map. Loaded from dynamic libraries.
std::unordered_map<std::string, void (*)()> PlayerClassUpdate;
// pointer to Freetype variables. (y'know for the fonts)
FreetypeClass* Freetypething;
// Map of ModelGroups. Refer to the ModelGroupClass class to see what that is.
std::unordered_map<std::string, ModelGroupClass> ModelGroupMap;
// pointer to Camera.
CameraClass* Camera;

std::unique_ptr<GlobalMapClass> GlobalMapStuff;

std::unique_ptr<RendererStuff> RendererGlobal;

std::unique_ptr<UIGlobalClass> UIGlobalStuff;

std::unique_ptr<GlobalNetworkClass> GlobalNetworkStuff;

void EntitySpawn() {}

unsigned int EntityMapEmptyIndex() {
  if (!Global->Entities.contains(0)) {
    return 0;
  }
  unsigned int i = 1;
  while (i != 0) {
    if (!Global->Entities.contains(i)) return i;
    i++;
  }
  return 0;
}

unsigned int ParticleMapEmptyIndex() {
  if (!Global->Particles.contains(0)) {
    return 0;
  }
  unsigned int i = 1;
  while (i != 0) {
    if (!Global->Particles.contains(i)) return i;
    i++;
  }
  return 0;
}