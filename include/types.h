#pragma once
typedef union {
  struct {
    float x, y;
  };
  float v[2];
} Vector2;

typedef union {
  struct {
    float x, y, z;
  };
  float v[3];
} Vector3;