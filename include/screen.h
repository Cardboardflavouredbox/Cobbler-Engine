#pragma once

#include "types.h"
struct ScreenPoint {
  Vector2 p;
  float dist;
  bool isbehindcamera = false;
};

ScreenPoint ToScreenSpace(Vector3 P);