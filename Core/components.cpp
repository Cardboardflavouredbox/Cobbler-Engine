#include "components.h"

#include "networkextern.h"
#include "particles.h"
#include "physics.h"
#include "ui.h"

// components that are called after update but before render
void componentsupdatelate() {
  // call npc entity late updates
  for (auto& i : Entities) {
    i.second->lateupdate();
  }
  // call player entity late updates
  for (auto& i : GlobalNetworkStuff->PlayerEntity) {
    i.second->lateupdate();
  }
  // call particle late updates
  for (auto& i : Particles) {
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
  for (auto& i : Entities) {
    i.second->update();
    EntityMove(i.second);
    i.second->deltatimelocal = 0;
  }
  // call player entity updates and EntityMove
  for (auto& i : GlobalNetworkStuff->PlayerEntity) {
    i.second->update();
    EntityMove(i.second);
    i.second->deltatimelocal = 0;
  }
  // call particle updates
  for (auto& i : Particles) {
    i.second->update();
  }
}