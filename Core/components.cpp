#include "components.h"

#include <SDL3/SDL_log.h>

#include "entity.h"
#include "lights.h"
#include "map.h"
#include "networkextern.h"
#include "particles.h"
#include "physics.h"
#include "ui.h"

// components that are called after update but before render
void componentsupdatelate() {
  // call npc entity late updates
  for (auto& i : Entities) {
    i.second->lualateupdate();
  }
  for (auto& i : GlobalNetworkStuff->PlayerNetStuff) {
    i.second.PlayerEntity->lualateupdate();
  }

  // call particle late updates
  for (auto& i : Particles) {
    i.second->lateupdate();
  }
  // call light late updates
  for (auto& i : Lights) {
    i.second->lateupdate();
  }
  // call UI components updates
  for (int i = 0; i < UIGlobalStuff->UIlist.size(); i++) {
    int len = UIGlobalStuff->UImap[UIGlobalStuff->UIlist[i]].size();
    for (int j = 0; j < len; j++) {
      UIGlobalStuff->UImap[UIGlobalStuff->UIlist[i]].at(j)->update();
    }
  }
}

// update components
void componentsupdate() {
  // call npc entity updates and EntityMove

  for (auto& [i, entity] : Entities) {
    entity->luaupdate();

    if (std::isnan(entity->position.x) || std::isnan(entity->position.y) ||
        std::isnan(entity->position.z))
      entity->hp = 0;
    if (entity->hp <= 0) {
      if (entity->respawn) {
        entity->hp = entity->maxhp;
        entity->position = GetRespawnPoint(entity->teamindex);
        entity->velocityvec3 = glm::vec3(0);
        entity->respawned = true;
      } else {
        EntitydeleteQueue.push(entity->EntityIndex);
      }
    } else {
      entity->respawned = false;
      EntityMove(entity);
      entity->deltatimelocal = 0;
    }
  }
  // call player entity updates and EntityMove
  for (auto& i : GlobalNetworkStuff->PlayerNetStuff) {
    i.second.PlayerEntity->luaupdate();

    if (i.second.PlayerEntity->hp <= 0) {
      i.second.PlayerEntity->hp = i.second.PlayerEntity->maxhp;
      i.second.PlayerEntity->position =
          GetRespawnPoint(i.second.PlayerEntity->teamindex);
      i.second.PlayerEntity->velocityvec3 = glm::vec3(0);
      i.second.PlayerEntity->respawned = true;
    } else {
      i.second.PlayerEntity->respawned = false;
      EntityMove(i.second.PlayerEntity);

      i.second.PlayerEntity->deltatimelocal = 0;
    }
  }

  if (GlobalNetworkStuff->RecvEntity != NULL) {
    EntityMove(GlobalNetworkStuff->RecvEntity);
    GlobalNetworkStuff->RecvEntity->deltatimelocal = 0;
  }
  // call particle updates
  for (auto& i : Particles) {
    i.second->update();
  }
  // call light updates
  for (auto& i : Lights) {
    i.second->update();
  }
}