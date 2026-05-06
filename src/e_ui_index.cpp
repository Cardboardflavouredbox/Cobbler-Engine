#include "extern.h"
#include "ui_index.h"

void changeUIindex() {
  if (Global->rendermode == 1)
    Global->UIindex = 1;
  else
    Global->UIindex = 0;
}