#include "net.h"

NetworkStuffClass* NetStuff;

bool CobblerInitNet() {
  if (!NET_Init()) return false;
  return true;
}

void CobblerQuitNet() { NET_Quit(); }