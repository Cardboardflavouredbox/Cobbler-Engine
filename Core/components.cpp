#include "components.h"

#include "extern.h"
#include "physics.h"
#include "ui.h"

// components that are called after update but before render
void componentsupdatelate() {
  // call npc entity late updates
  for (int i = 0; i < Global->Entities.size(); i++) {
    Global->Entities[i]->lateupdate();
  }
  // call player entity late updates
  for (auto& i : Global->PlayerEntity) {
    i.second->lateupdate();
  }
  // call UI components updates
  for (int i = 0; i < Global->UIlist.size(); i++) {
    int len = Global->UImap[Global->UIlist[i]].size();
    for (int j = 0; j < len; j++) {
      Global->UImap[Global->UIlist[i]].at(j)->update();
    }
  }
}

// update components
void componentsupdate() {
  // call npc entity updates and EntityMove
  for (int i = 0; i < Global->Entities.size(); i++) {
    Global->Entities[i]->update();
    EntityMove(Global->Entities[i]);
    Global->Entities[i]->deltatimelocal = 0;
  }
  // call player entity updates and EntityMove
  for (auto& i : Global->PlayerEntity) {
    i.second->update();
    EntityMove(i.second);
    i.second->deltatimelocal = 0;
  }
}