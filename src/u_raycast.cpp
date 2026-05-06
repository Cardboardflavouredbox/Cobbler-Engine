#include <glm/glm.hpp>

#include "raycast.h"

// https://gamedev.stackexchange.com/a/5589
bool RayTriCheck(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, glm::vec3 R1,
                 glm::vec3 R2, glm::vec3& PIP) {
  glm::vec3 Normal, IntersectPos;

  Normal = glm::cross((P2 - P1), (P3 - P1));

  // Find distance from LP1 and LP2 to the plane defined by the triangle
  float Dist1 = glm::dot(Normal, (R1 - P1));
  float Dist2 = glm::dot(Normal, (R2 - P1));

  if ((Dist1 * Dist2) >= 0.0f) {
    // SFLog(@"no cross");
    return false;
  }  // line doesn't cross the triangle.

  if (Dist1 == Dist2) {
    // SFLog(@"parallel");
    return false;
  }  // line and plane are parallel

  // Find point on the line that intersects with the plane
  IntersectPos = ((R1 + (R2 - R1)) * (-Dist1 / (Dist2 - Dist1)));

  // Find if the interesection point lies inside the triangle by testing it
  // against all edges
  glm::vec3 vTest;

  vTest = glm::cross(Normal, (P2 - P1));
  if (glm::dot(vTest, (IntersectPos - P1)) < 0.0f) {
    // SFLog(@"no intersect P2-P1");
    return false;
  }

  vTest = glm::cross(Normal, (P3 - P2));
  if (glm::dot(vTest, (IntersectPos - P2)) < 0.0f) {
    // SFLog(@"no intersect P3-P2");
    return false;
  }

  vTest = glm::cross(Normal, (P1 - P3));
  if (glm::dot(vTest, (IntersectPos - P1)) < 0.0f) {
    // SFLog(@"no intersect P1-P3");
    return false;
  }

  PIP = IntersectPos;

  return true;
}