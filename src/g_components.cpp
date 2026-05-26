#include "components.h"
#include "entityphysics.h"

void componentsupdate() {
  for (int i = 0; i < Global->Entities.size(); i++) {
    EntityMove(Global->Entities[i]);
    Global->Entities[i]->update();
  }
}