#include "raycast.h"

// https://gamedev.stackexchange.com/a/5589
bool RayTriCheck(Vector3 P1, Vector3 P2, Vector3 P3, Vector3 R1, Vector3 R2,
                 Vector3& PIP) {
  Vector3 Normal, IntersectPos;

  Normal = Vector3Cross(subVec3(P2, P1), subVec3(P3, P1));

  // Find distance from LP1 and LP2 to the plane defined by the triangle
  float Dist1 = Vector3Dot(Normal, subVec3(R1, P1));
  float Dist2 = Vector3Dot(Normal, subVec3(R2, P1));

  if ((Dist1 * Dist2) >= 0.0f) {
    // SFLog(@"no cross");
    return false;
  }  // line doesn't cross the triangle.

  if (Dist1 == Dist2) {
    // SFLog(@"parallel");
    return false;
  }  // line and plane are parallel

  // Find point on the line that intersects with the plane
  IntersectPos =
      multiplyVec3(addVec3(R1, subVec3(R2, R1)), (-Dist1 / (Dist2 - Dist1)));

  // Find if the interesection point lies inside the triangle by testing it
  // against all edges
  Vector3 vTest;

  vTest = Vector3Cross(Normal, subVec3(P2, P1));
  if (Vector3Dot(vTest, subVec3(IntersectPos, P1)) < 0.0f) {
    // SFLog(@"no intersect P2-P1");
    return false;
  }

  vTest = Vector3Cross(Normal, subVec3(P3, P2));
  if (Vector3Dot(vTest, subVec3(IntersectPos, P2)) < 0.0f) {
    // SFLog(@"no intersect P3-P2");
    return false;
  }

  vTest = Vector3Cross(Normal, subVec3(P1, P3));
  if (Vector3Dot(vTest, subVec3(IntersectPos, P1)) < 0.0f) {
    // SFLog(@"no intersect P1-P3");
    return false;
  }

  PIP = IntersectPos;

  return true;
}