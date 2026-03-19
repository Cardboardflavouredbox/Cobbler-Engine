#include "types.h"

#include <cmath>

Vector2 addVec2(Vector2 input1, Vector2 input2) {
  return Vector2({input1.x + input2.x, input1.y + input2.y});
}

Vector3 addVec3(Vector3 input1, Vector3 input2) {
  return Vector3(
      {input1.x + input2.x, input1.y + input2.y, input1.z + input2.z});
}

Vector3 subVec3(Vector3 input1, Vector3 input2) {
  return Vector3(
      {input1.x - input2.x, input1.y - input2.y, input1.z - input2.z});
}

Vector2 multiplyVec2(Vector2 inputvec2, float multvalue) {
  return Vector2({inputvec2.x * multvalue, inputvec2.y * multvalue});
}

Vector3 multiplyVec3(Vector3 inputvec2, float multvalue) {
  return Vector3({inputvec2.x * multvalue, inputvec2.y * multvalue,
                  inputvec2.z * multvalue});
}

float Vector3Dot(Vector3 P1, Vector3 P2) {
  return P1.x * P2.x + P1.y * P2.y + P1.z * P2.z;
}

Vector3 Vector3Normalize(Vector3 input) {
  float temp =
      std::sqrt(input.x * input.x + input.y * input.y + input.z * input.z);
  if (temp == 0) return Vector3({0, 0, 0});
  return multiplyVec3(
      input,
      1 / std::sqrt(input.x * input.x + input.y * input.y + input.z * input.z));
}

float getVec3dist(Vector3 p1, Vector3 p2) {
  float x = p2.x - p1.x;
  float y = p2.y - p1.y;
  float z = p2.z - p1.z;
  return std::sqrt(x * x + y * y + z * z);
}