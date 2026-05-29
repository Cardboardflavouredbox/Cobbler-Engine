#include "physics.h"

#include <SDL3/SDL_log.h>

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

float getdistancething(glm::vec3 P) {
  glm::vec3 p1 = P - Camera->position;
  float ps = std::sin(Camera->dir.x * PI / 180.f);
  float pc = std::cos(Camera->dir.x * PI / 180.f);
  glm::quat q =
      glm::angleAxis(glm::radians(Camera->dir.y), glm::vec3(-pc, -ps, 0.0f));

  p1 = q * p1;
  return p1.y * pc - p1.x * ps;
}

bool CapsuleTriCheck(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, glm::vec3 R1,
                     glm::vec3 R2, float radius) {
  glm::vec3 Normal, IntersectPos;

  return true;
}

bool movecollisioncheck(glm::vec3 hitbox[], glm::vec3 checkposition,
                        float radius) {  // true if collided
  for (int i = 0; i < Global->mapfaces.size(); i++) {
    if (CapsuleTriCheck(Global->Points[Global->mapfaces[i].points[0]],
                        Global->Points[Global->mapfaces[i].points[1]],
                        Global->Points[Global->mapfaces[i].points[2]],
                        hitbox[0] + checkposition, hitbox[1] + checkposition,
                        radius)) {
      SDL_Log("%d", i);
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

  int temp = (int(std::sqrt(tempmove.x * tempmove.x + tempmove.y * tempmove.y) /
                  tempentity->hitboxradius) +
              1);

  for (int i = 0; i < temp; i++) {
    tempposition.x += tempmove.x / (float)temp;
    tempposition.y += tempmove.y / (float)temp;
    if (!movecollisioncheck(tempentity->hitbox, tempposition,
                            tempentity->hitboxradius)) {
      moveresult.x += tempmove.x / (float)temp;
      moveresult.y += tempmove.y / (float)temp;
    } else
      break;
  }

  tempposition = moveresult + tempentity->position;
  temp = std::abs(tempmove.z) / tempentity->hitboxradius + 1;
  for (int i = 0; i < temp; i++) {
    tempposition.z += tempmove.z / (float)temp;
    // SDL_Log("%f %f %f", tempposition.x, tempposition.y, tempposition.z);
    if (!movecollisioncheck(tempentity->hitbox, tempposition,
                            tempentity->hitboxradius)) {
      tempentity->IsGrounded = false;
      moveresult.z += tempmove.z / (float)temp;
    } else {
      tempentity->IsGrounded = true;
      tempentity->velocityvec3.z = -2.f;
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