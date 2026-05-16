#include "extern.h"
#include "settingsmenu.h"
#include "ui_index.h"

int menuindex = 0;
int currentmenusize = 5;

void UIupdate() {
  if (P1Inputs->Enter == 2) {
  }
  if (P1Inputs->W == 2) {
    menuindex--;
    if (menuindex < 0) menuindex = currentmenusize - 1;
  }
  if (P1Inputs->S == 2) {
    menuindex++;
    if (menuindex >= currentmenusize) menuindex = 0;
  }
}

void changeUIindex() {
  UIupdate();
  if (Global->pause || Global->isopeningfile) {
    Global->UIlist[0] = "Pause";
  } else
    Global->UIlist[0] = "Pistol";
}