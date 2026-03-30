#include "components.h"

#include "extern.h"
#include "ui.h"

void componentsupdate() {}

void componentsupdatelate() {
  std::deque<UIthing*>* tempdeque = &Global->UImap[0];
  int len = tempdeque->size();
  for (int i = 0; i < len; i++) {
    tempdeque->at(i)->update();
  }
}