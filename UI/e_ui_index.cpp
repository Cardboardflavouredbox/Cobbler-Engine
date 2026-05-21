#include "extern.h"

void changeUIindex() {
  if (Editor->currentlyselectedface > -1)
    Global->UIlist[0] = "Face";
  else
    Global->UIlist[0] = "Point";
}