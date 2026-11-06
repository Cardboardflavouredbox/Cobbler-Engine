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

std::map<uint32_t, EntityLuaStuff*> EntityLuaMap;

uint32_t EntityLuaMapEmptyIndex() {
  if (!EntityLuaMap.contains(0)) {
    return 0;
  }
  uint32_t i = 1;
  while (i != 0) {
    if (!EntityLuaMap.contains(i)) return i;
    i++;
  }
  return 0;
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

  EntityLuaStuff* tempLua = new EntityLuaStuff();
  tempLua->lua.open_libraries(sol::lib::base);

  tempLua->lua.new_usertype<Entity>(
      "Entity", sol::constructors<Entity()>(), "respawn", &Entity::respawn,
      "movespeed", &Entity::movespeed, "jumpheight", &Entity::jumpheight,
      "cameraoffset", &Entity::cameraoffset, "hitboxradius",
      &Entity::hitboxradius, "hitboxlength", &Entity::hitboxlength, "teamindex",
      &Entity::teamindex, "maxhp", &Entity::maxhp, "hp", &Entity::hp);

  tempLua->lua["entity"] = this;

  Modelthing = new Modeltransform();

  tempLua->lua["ModeltransName"] = &Modelthing->name;

  float modeltranssize = 1.f;
  tempLua->lua["ModeltransSize"] = &modeltranssize;

  std::string scriptpath =
      Global->GameFolder.string() + "/class/" + path + "/entitymain.lua";

  std::filesystem::path entry = std::filesystem::path(scriptpath);
  if (entry.has_extension() && entry.extension() == ".lua") {
    tempLua->lua.script_file(scriptpath);
  } else {
    scriptpath =
        Global->GameFolder.string() + "/entity/" + path + "/entitymain.lua";
    entry = std::filesystem::path(scriptpath);
    if (entry.has_extension() && entry.extension() == ".lua") {
      tempLua->lua.script_file(scriptpath);
    } else {
      SDL_Log("No Entity named %s!", path.c_str());
    }
  }

  sol::table actionsthing = tempLua->lua["SetActions"];

  std::vector<int> tempvec = actionsthing.as<std::vector<int>>();

  Modelthing->SetActions(tempvec);

  tempLua->lua["luasetup"]();

  tempLua->luaupdate = tempLua->lua["luaupdate"];
  tempLua->lualateupdate = tempLua->lua["lualateupdate"];

  Modelthing->size = glm::vec3(modeltranssize);

  tempLua->lua.collect_garbage();

  EntityLuaMap[EntityLuaMapEmptyIndex()] = tempLua;
}