#include "extern.h"
#include "ui_index.h"

void changeUIindex() {
  if (Editor->currentlyselectedface > -1)
    Global->UIname = "Face";
  else
    Global->UIname = "Point";
}