#pragma once
struct Vector2 {
  float x, y;
};

struct Vector3 {
  float x, y, z;
};

Vector3 multiplyVec3(Vector3 inputvec2, float multvalue);

Vector3 Vector3Normalize(Vector3 input);

Vector3 addVec3(Vector3 input1, Vector3 input2);