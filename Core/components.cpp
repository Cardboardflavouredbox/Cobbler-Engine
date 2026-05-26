#include "components.h"

#include "extern.h"
#include "ui.h"

void componentsupdatelate() {
  for (int i = 0; i < Global->Entities.size(); i++) {
    Global->Entities[i]->lateupdate();
  }
  for (int i = 0; i < Global->UIlist.size(); i++) {
    int len = Global->UImap[Global->UIlist[i]].size();
    for (int j = 0; j < len; j++) {
      Global->UImap[Global->UIlist[i]].at(j)->update();
    }
  }
}