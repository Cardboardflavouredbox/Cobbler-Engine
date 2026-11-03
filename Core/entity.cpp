#include "entity.h"

#include <SDL3/SDL_log.h>

#include <cmath>

#include "deltaTime.h"
#include "extern.h"
#include "model.h"
#include "render.h"

void Entity::setup(uint32_t Index, const std::string& path) {
  EntityIndex = Index;
  lua.open_libraries(sol::lib::base);

  sol::usertype<Entity> entity_type =
      lua.new_usertype<Entity>("Entity", sol::constructors<Entity()>());

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

  entity_type["respawn"] = &Entity::respawn;
  entity_type["movespeed"] = &Entity::movespeed;
  entity_type["jumpheight"] = &Entity::jumpheight;
  entity_type["cameraoffset"] = &Entity::cameraoffset;
  entity_type["hitboxradius"] = &Entity::hitboxradius;
  entity_type["hitbox"] = &Entity::hitbox;
  entity_type["teamindex"] = &Entity::teamindex;
  entity_type["maxhp"] = &Entity::maxhp;
  entity_type["hp"] = &Entity::hp;

  lua.script_file(Global->GameFolder.string() + "/" + path + "/entitymain.lua");

  lua["luasetup"]();

  luaupdate = lua["luaupdate"];
  lualateupdate = lua["lualateupdate"];
}