#include "components.h"

#include "extern.h"
#include "ui.h"

void componentsupdatelate() {
  for (int i = 0; i < Global->UIlist.size(); i++) {
    std::vector<std::shared_ptr<UIthing>>* tempvector =
        &Global->UImap[Global->UIlist[i]];
    int len = tempvector->size();
    for (int i = 0; i < len; i++) {
      tempvector->at(i)->update();
    }
  }
}