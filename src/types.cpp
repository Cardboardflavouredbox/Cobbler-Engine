#include "types.h"

Vector3& Vector3::operator+(const Vector3& v) {
  Vector3 temp = Vector3({x + v.x, y + v.y, z + v.z});
  return *temp;
}

Vector3& Vector3::operator-(const Vector3& v) {
  Vector3 temp = Vector3({x - v.x, y - v.y, z - v.z});
  return *temp;
}