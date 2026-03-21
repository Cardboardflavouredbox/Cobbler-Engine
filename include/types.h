#pragma once
struct Vector2 {
  float x, y;
};

struct Vector3 {
  float x, y, z;
};

struct Capsule {
  Vector3 offset;
  float radius, height, reduction;
};

Vector3 multiplyVec3(Vector3 inputvec2, float multvalue);

Vector2 multiplyVec2(Vector2 inputvec2, float multvalue);

Vector3 Vector3Normalize(Vector3 input);

Vector2 addVec2(Vector2 input1, Vector2 input2);

Vector3 addVec3(Vector3 input1, Vector3 input2);

float Vector3Dot(Vector3 P1, Vector3 P2);

Vector3 subVec3(Vector3 input1, Vector3 input2);

Vector3 Vector3Normalize(Vector3 input);

float getVec3dist(Vector3 p1, Vector3 p2);

Vector3 Vector3Cross(Vector3 a, Vector3 b);