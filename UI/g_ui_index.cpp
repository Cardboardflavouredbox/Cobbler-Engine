#include "extern.h"
#include "ui_index.h"

void changeUIindex() {
  if (Global->pause || Global->isopeningfile) {
    Global->UIname = "Pause";
  } else
    Global->UIname = "Pistol";
}