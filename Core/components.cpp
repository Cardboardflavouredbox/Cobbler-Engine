#include "components.h"
#include "extern.h"
#include "ui.h"

void componentsupdatelate() {
  std::deque<UIthing*>* tempdeque = &Global->UImap[Global->UIname];
  int len = tempdeque->size();
  for (int i = 0; i < len; i++) {
    tempdeque->at(i)->update();
  }
}