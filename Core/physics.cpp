#include "physics.h"

#include <glm/glm.hpp>

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

// https://stackoverflow.com/a/74395029
glm::vec3 closestPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b,
                               glm::vec3 c) {
  glm::vec3 ab = b - a;
  glm::vec3 ac = c - a;
  glm::vec3 ap = p - a;

  float d1 = dot(ab, ap);
  float d2 = dot(ac, ap);
  if (d1 <= 0.f && d2 <= 0.f) return a;  // #1

  glm::vec3 bp = p - b;
  float d3 = dot(ab, bp);
  float d4 = dot(ac, bp);
  if (d3 >= 0.f && d4 <= d3) return b;  // #2

  glm::vec3 cp = p - c;
  float d5 = dot(ab, cp);
  float d6 = dot(ac, cp);
  if (d6 >= 0.f && d5 <= d6) return c;  // #3

  float vc = d1 * d4 - d3 * d2;
  if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f) {
    float v = d1 / (d1 - d3);
    return a + v * ab;  // #4
  }

  float vb = d5 * d2 - d1 * d6;
  if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f) {
    float v = d2 / (d2 - d6);
    return a + v * ac;  // #5
  }

  float va = d3 * d6 - d5 * d4;
  if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f) {
    float v = (d4 - d3) / ((d4 - d3) + (d5 - d6));
    return b + v * (c - b);  // #6
  }

  float denom = 1.f / (va + vb + vc);
  float v = vb * denom;
  float w = vc * denom;
  return a + v * ab + w * ac;  // #0
}

bool CapsuleTriCheck(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, glm::vec3 R1,
                     glm::vec3 R2, float radius) {
  int len = (glm::distance(R1, R2) / radius) + 1;
  for (int i = 0; i < len; i++) {
    glm::vec3 temp = R1 + (R2 - R1) * (float)i / (float)len;
    glm::vec3 closest = closestPointTriangle(temp, P1, P2, P3);
    if (glm::distance(temp, closest) < radius) return true;
  }
  return false;
}

bool movecollisioncheck(glm::vec3 hitbox[], glm::vec3 checkposition,
                        float radius) {  // true if collided
  for (int i = 0; i < Global->mapfaces.size(); i++) {
    if (CapsuleTriCheck(Global->Points[Global->mapfaces[i].points[0]],
                        Global->Points[Global->mapfaces[i].points[1]],
                        Global->Points[Global->mapfaces[i].points[2]],
                        hitbox[0] + checkposition, hitbox[1] + checkposition,
                        radius)) {
      return true;
    }
  }
  return false;
}

void EntityMove(Entity* tempentity) {
  tempentity->velocityvec3.z -=
      tempentity->gravity * (Global->deltaTime / 10.f);

  glm::vec3 tempmove = ((tempentity->movevec3 + tempentity->velocityvec3) *
                        (Global->deltaTime / 10.f));
  glm::vec3 tempposition = tempentity->position,
            moveresult = glm::vec3({0, 0, 0});

  int temp = std::abs(tempmove.x) / tempentity->hitboxradius * 128 + 1;

  for (int i = 0; i < temp; i++) {
    tempposition.x += tempmove.x / (float)temp;
    if (!movecollisioncheck(tempentity->hitbox, tempposition,
                            tempentity->hitboxradius)) {
      moveresult.x += tempmove.x / (float)temp;
    } else
      break;
  }

  temp = std::abs(tempmove.y) / tempentity->hitboxradius * 128 + 1;
  tempposition = moveresult + tempentity->position;
  for (int i = 0; i < temp; i++) {
    tempposition.y += tempmove.y / (float)temp;
    if (!movecollisioncheck(tempentity->hitbox, tempposition,
                            tempentity->hitboxradius)) {
      moveresult.y += tempmove.y / (float)temp;
    } else
      break;
  }

  tempposition = moveresult + tempentity->position;
  temp = (std::abs(tempmove.z) / tempentity->hitboxradius) * 128 + 1;
  for (int i = 0; i < temp; i++) {
    tempposition.z += tempmove.z / (float)temp;
    if (!movecollisioncheck(tempentity->hitbox, tempposition,
                            tempentity->hitboxradius)) {
      tempentity->IsGrounded = false;
      moveresult.z += tempmove.z / (float)temp;
    } else {
      tempentity->IsGrounded = true;
      tempentity->velocityvec3.z = -4.f;
      break;
    }
  }

  tempentity->position = (tempentity->position + moveresult);
  tempentity->movevec3 = glm::vec3({0, 0, 0});

  if (tempentity->velocityvec3.x > 0) {
    tempentity->velocityvec3.x -= Global->deltaTime;
    if (tempentity->velocityvec3.x < 0) tempentity->velocityvec3.x = 0;
  } else if (tempentity->velocityvec3.x < 0) {
    tempentity->velocityvec3.x += Global->deltaTime;
    if (tempentity->velocityvec3.x > 0) tempentity->velocityvec3.x = 0;
  }

  if (tempentity->velocityvec3.y > 0) {
    tempentity->velocityvec3.y -= Global->deltaTime;
    if (tempentity->velocityvec3.y < 0) tempentity->velocityvec3.y = 0;
  } else if (tempentity->velocityvec3.y < 0) {
    tempentity->velocityvec3.y += Global->deltaTime;
    if (tempentity->velocityvec3.y > 0) tempentity->velocityvec3.y = 0;
  }
}