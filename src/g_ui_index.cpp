#include "extern.h"
#include "ui_index.h"

void changeUIindex() {
  if (Global->pause || Global->isopeningfile) {
    Global->UIindex = 1;
  } else
    Global->UIindex = 0;
}