#include <cmath>

#include "entity.h"
#include "extern.h"
#include "global.h"
#include "update.h"
void update() {
  Camera->dir.x += -2 * P1Inputs->Mouse.x;
  Camera->dir.y += -2 * P1Inputs->Mouse.y;
  P1Inputs->Mouse.x = 0;
  P1Inputs->Mouse.y = 0;

  float ps = std::sin(Camera->dir.x * 3.14 / 180.0);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.0);
  Vector3 tempmove = Vector3({0, 0, 0});
  if (P1Inputs->A > 0) {
    tempmove.x -= std::sin((Camera->dir.x + 90) * 3.14 / 180.0);
    tempmove.y += std::cos((Camera->dir.x + 90) * 3.14 / 180.0);
  }
  if (P1Inputs->D > 0) {
    tempmove.x -= std::sin((Camera->dir.x - 90) * 3.14 / 180.0);
    tempmove.y += std::cos((Camera->dir.x - 90) * 3.14 / 180.0);
  }
  if (P1Inputs->W > 0) {
    tempmove.x -= ps;
    tempmove.y += pc;
  }
  if (P1Inputs->S > 0) {
    tempmove.x += ps;
    tempmove.y -= pc;
  }
}