#include "entity.h"

#include <SDL3/SDL_log.h>

#include <cmath>

#include "deltaTime.h"
#include "extern.h"
#include "model.h"
#include "render.h"

// Entities map. Contains all the npc Entities.
std::map<uint32_t, Entity*> Entities;
// Queue that contains all the Entities to delete this frame.
std::queue<uint32_t> EntitydeleteQueue;
// Entities Pool.
std::unordered_map<std::string, std::queue<Entity*>> EntityPool;

std::vector<EntityLuaStuff*> EntityLuaVec;

void Entity::setup(uint32_t Index, const std::string& path) {
  EntityIndex = Index;

  EntityLuaStuff* tempLua = new EntityLuaStuff();
  tempLua->lua.open_libraries(sol::lib::base);

  // Modelthing = new Modeltransform();
  // Modelthing->name = "PlayerMain";
  // Modeltransform::action actionthing;
  // actionthing.pose.posecode.push_back(getposeindex("Idle"));
  // Modelthing->actions.push_back(actionthing);
  // actionthing.pose.posecode.front() = getposeindex("AimPistolDown");
  // actionthing.pose.posecode.push_back(getposeindex("AimPistol"));
  // actionthing.pose.posecode.push_back(getposeindex("AimPistolUp"));
  // actionthing.pose.lerpamount = 1.f;
  // Modelthing->actions.push_back(actionthing);
  // Modelthing->size = glm::vec3(1.f / 2.13447647059f);

  tempLua->lua["entity"] = this;

  tempLua->lua.script_file(Global->GameFolder.string() + "/class/" + path +
                           "/entitymain.lua");

  tempLua->lua["luasetup"]();

  tempLua->luaupdate = tempLua->lua["luaupdate"];
  tempLua->lualateupdate = tempLua->lua["lualateupdate"];
  EntityLuaVec.push_back(tempLua);
}