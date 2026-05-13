#include "extern.h"
#include "ui_index.h"

void changeUIindex() {
  if (Editor->UIindex == 1)
    Global->UIindex = 1;
  else
    Global->UIindex = 0;
}