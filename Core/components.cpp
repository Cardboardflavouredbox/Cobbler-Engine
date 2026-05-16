#include "components.h"

#include "extern.h"
#include "ui.h"

void componentsupdatelate() {
  for (int i = 0; i < Global->UIlist.size(); i++) {
    std::deque<UIthing*>* tempdeque = &Global->UImap[Global->UIlist[i]];
    int len = tempdeque->size();
    for (int i = 0; i < len; i++) {
      tempdeque->at(i)->update();
    }
  }
}