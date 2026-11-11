#include "entity.h"

#include <SDL3/SDL_log.h>

#include <cmath>

#include "deltaTime.h"
#include "extern.h"
#include "lights.h"
#include "luathing.h"
#include "model.h"
#include "networkextern.h"
#include "physics.h"
#include "pi.h"
#include "render.h"

// Entities map. Contains all the npc Entities.
std::map<uint32_t, Entity*> Entities;
// Queue that contains all the Entities to delete this frame.
std::queue<uint32_t> EntitydeleteQueue;
// Entities Pool.
std::unordered_map<std::string, std::queue<Entity*>> EntityPool;

void RadiusDamageEntity(glm::vec3 position, Entity* tempentity,
                        uint64_t TargetEntityIndex, bool TargetIsPlayer,
                        int teamindex, uint64_t EntityIndex, float radius = 7.f,
                        float damage = 40.f, float pushback = 28.5f,
                        Entity* SourceEntity = NULL) {
  raycheckresult temp = capsuleraycheck(
      position, position,
      glm::vec3(0, 0, tempentity->hitboxradius) + tempentity->position,
      glm::vec3(0, 0, tempentity->hitboxlength - tempentity->hitboxradius) +
          tempentity->position);
  glm::vec3 normal = glm::normalize(temp.B - temp.A);

  if (temp.dist < radius + tempentity->hitboxradius &&
      ((SourceEntity != NULL && SourceEntity == tempentity) ||
       tempentity->teamindex != teamindex)) {
    normal.x *= 2;
    normal.y *= 2;

    float amount = (tempentity->hitboxradius + radius - temp.dist) /
                   (tempentity->hitboxradius + radius);

    if (amount > 0.75f) amount = 0.75f;
    amount += 0.25f;

    if (tempentity->velocityvec3.z < 0) tempentity->velocityvec3.z = 0;
    tempentity->velocityvec3 += normal * amount * pushback / tempentity->weight;
    EntityDamageInfo infothing;

    infothing.damage = damage * amount;

    if (SourceEntity != NULL && SourceEntity == tempentity) {
      infothing.damage *= 3.f / 5.f;
    }

    infothing.FromPlayer = false;
    infothing.SourceEntityIndex = EntityIndex;
    infothing.TargetEntityIndex = TargetEntityIndex;
    infothing.TargetIsPlayer = TargetIsPlayer;
    for (int i = 0; i < 3; i++) infothing.pushdir[i] = 0;
    DamageEntity(infothing, false);
  }
}

void RadiusDamage(Entity* entity, glm::vec3 position, int teamindex,
                  float radius = 7.f, float damage = 40.f,
                  float pushback = 28.5f) {
  uint32_t lightindex = LightMapEmptyIndex();
  Lights[lightindex] = new TempLight(lightindex, 1.f);
  Lights[lightindex]->position = position;
  Lights[lightindex]->color[0] = 1;
  Lights[lightindex]->color[1] = 0.9f;
  Lights[lightindex]->color[2] = 0;
  ParticleSpawn(ParticleSpawnInfo{0, {position[0], position[1], position[2]}},
                true);
  for (auto& i : Entities) {
    RadiusDamageEntity(position, i.second, i.first, false, teamindex,
                       entity->EntityIndex, radius, damage, pushback,
                       entity->SourceEntity);
  }
  for (auto& i : GlobalNetworkStuff->PlayerNetStuff) {
    RadiusDamageEntity(position, i.second.PlayerEntity, i.first, true,
                       teamindex, entity->EntityIndex, radius, damage, pushback,
                       entity->SourceEntity);
  }
}

glm::vec3 setvec3dir(glm::vec2 dir) {
  float ps = std::sin(dir.x * PI / 180.f);
  float pc = std::cos(dir.x * PI / 180.f);
  float what = std::cos(dir.y * PI / 180.f);
  return glm::normalize(
      glm::vec3{-ps * what, pc * what, std::sin(dir.y * PI / 180.f)});
}

glm::quat setquatfromdir(glm::vec3 dir) {
  return glm::quatLookAt(dir, glm::vec3(0, 0, 1)) *
         glm::quatLookAt(glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));
}

void Modeltransform::SetActions(const std::vector<int>& input) {
  for (int i = 0; i < input.size(); i++) {
    Modeltransform::action actionthing;
    for (int j = 0; j < input[i]; j++) actionthing.pose.posecode.push_back(0);
    actions.push_back(actionthing);
  }
}

void Entity::setup(uint32_t Index, std::string path) {
  EntityIndex = Index;

  lua.open_libraries(sol::lib::base, sol::lib::math);

  LuaSet(&lua);

  Modelthing = new Modeltransform();

  lua["entity"] = this;
  lua["Modelthing"] = Modelthing;

  lua["Modelthingpos"] = &Modelthing->position;
  lua["Modelthingdir"] = &Modelthing->lookdir;
  lua["Modelthingsize"] = &Modelthing->size;
  lua["Modelthingrot"] = &Modelthing->rot;

  lua["entitypos"] = &position;
  lua["entitydir"] = &dir;

  lua["setvec3dir"] = setvec3dir;
  lua["setquatfromdir"] = setquatfromdir;
  lua["RadiusDamage"] = RadiusDamage;

  lua["getposeindex"] = getposeindex;

  std::string scriptpath =
      Global->GameFolder.string() + "/class/" + path + "/entitymain.lua";

  std::filesystem::path entry = std::filesystem::path(scriptpath);

  if (std::filesystem::exists(entry)) {
    lua.script_file(scriptpath);
  } else {
    scriptpath =
        Global->GameFolder.string() + "/entities/" + path + "/entitymain.lua";
    entry = std::filesystem::path(scriptpath);
    if (std::filesystem::exists(entry)) {
      lua.script_file(scriptpath);
    } else {
      SDL_Log("No Entity named %s!", path.c_str());
    }
  }

  sol::table actionsthing = lua["SetActions"];

  std::vector<int> tempvec = actionsthing.as<std::vector<int>>();

  Modelthing->SetActions(tempvec);

  lua["luasetup"]();

  luaupdate = lua["luaupdate"];
  lualateupdate = lua["lualateupdate"];
}