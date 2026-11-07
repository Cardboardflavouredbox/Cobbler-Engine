#include "entity.h"

#include <SDL3/SDL_log.h>

#include <cmath>

#include "deltaTime.h"
#include "extern.h"
#include "luathing.h"
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

  LuaSet(&tempLua->lua);

  Modelthing = new Modeltransform();

  tempLua->lua["entity"] = this;
  tempLua->lua["Modelthing"] = Modelthing;

  tempLua->lua["Modelthingpos"] = &Modelthing->position;
  tempLua->lua["Modelthingdir"] = &Modelthing->lookdir;
  tempLua->lua["Modelthingsize"] = &Modelthing->size;
  tempLua->lua["Modelthingrot"] = &Modelthing->rot;

  tempLua->lua["entitypos"] = &position;
  tempLua->lua["entitydir"] = &dir;

  std::string scriptpath =
      Global->GameFolder.string() + "/class/" + path + "/entitymain.lua";

  std::filesystem::path entry = std::filesystem::path(scriptpath);

  if (std::filesystem::exists(entry)) {
    tempLua->lua.script_file(scriptpath);
  } else {
    scriptpath =
        Global->GameFolder.string() + "/entities/" + path + "/entitymain.lua";
    entry = std::filesystem::path(scriptpath);
    if (std::filesystem::exists(entry)) {
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

  tempLua->entity = this;

  EntityLuaMap[EntityLuaMapEmptyIndex()] = tempLua;
}