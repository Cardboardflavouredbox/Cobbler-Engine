#include "extern.h"
#include "ui_index.h"

void changeUIindex() {
  if (Global->pause || Global->isopeningfile) {
    Global->UIname = "pause";
  } else
    Global->UIname = "default";
}