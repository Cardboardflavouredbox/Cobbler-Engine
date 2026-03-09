#pragma once
struct Vector2 {
  float x, y;
};
struct Vector3 {
  float x, y, z;
  Vector3& operator+(const Vector3& v);
  Vector3& operator-(const Vector3& v);
};