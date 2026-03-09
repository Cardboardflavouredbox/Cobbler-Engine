#pragma once
struct Vector2 {
  float x, y;
};
struct Vector3 {
  float x, y, z;
  Vector3 operator+(const Vector3& v) {
    return Vector3({x + v.x, y + v.y, z + v.z});
  }

  Vector3 operator-(const Vector3& v) {
    return Vector3({x - v.x, y - v.y, z - v.z});
  }
};