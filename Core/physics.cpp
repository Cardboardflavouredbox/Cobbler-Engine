#include "physics.h"

#include <SDL3/SDL_log.h>

#include <cmath>
#include <glm/glm.hpp>

#include "deltaTime.h"

// Ray and Triangle collision check function.
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

// checks if capsule and ray overlaps.
// pretty much compares two rays and see if the minimum distance is shorter than
// the radius.
// https://stackoverflow.com/a/18994296
// https://stackoverflow.com/questions/2824478/shortest-distance-between-two-line-segments#comment79231859_18994296
raycheckresult capsuleraycheck(glm::vec3 a0, glm::vec3 a1, glm::vec3 b0,
                               glm::vec3 b1) {
  raycheckresult result;
  // Calculate denomitator
  glm::vec3 A = a1 - a0;
  glm::vec3 B = b1 - b0;
  float magA = glm::length(A);
  float magB = glm::length(B);

  glm::vec3 _A = A / magA;
  glm::vec3 _B = B / magB;

  glm::vec3 cross = glm::cross(_A, _B);
  float denom = glm::length(cross);
  denom *= denom;

  // If lines are parallel (denom=0) test if lines overlap.
  // If they don't overlap then there is a closest point solution.
  // If they do overlap, there are infinite closest positions, but there is a
  // closest distance
  if (denom == 0) {
    float d0 = glm::dot(_A, (b0 - a0));

    // Overlap only possible with clamping

    float d1 = glm::dot(_A, (b1 - a0));

    // Is segment B before A?
    if (d0 <= 0 && 0 >= d1) {
      if (std::fabsf(d0) < std::fabsf(d1)) {
        result.A = a0;
        result.B = b0;
        result.dist = glm::length(a0 - b0);

        return result;
      }
      result.A = a0;
      result.B = b1;
      result.dist = glm::length(a0 - b1);

      return result;
    }
    // Is segment B after A?
    else if (d0 >= magA && magA <= d1) {
      if (std::fabsf(d0) < std::fabsf(d1))

      {
        result.A = a1;
        result.B = b0;
        result.dist = glm::length(a1 - b0);

        return result;
      }
      result.A = a1;
      result.B = b1;
      result.dist = glm::length(a1 - b1);

      return result;
    }

    // Segments overlap, return distance between parallel segments
    result.A = a1;
    result.B = b1;
    result.dist = glm::length(((d0 * _A) + a0) - b0);
    return result;
  }

  // Lines criss-cross: Calculate the projected closest points
  glm::vec3 t = (b0 - a0);
  float detA = glm::determinant(glm::mat3(t, _B, cross));
  float detB = glm::determinant(glm::mat3(t, _A, cross));

  float t0 = detA / denom;
  float t1 = detB / denom;

  glm::vec3 pA = a0 + (_A * t0);  // Projected closest point on segment A
  glm::vec3 pB = b0 + (_B * t1);  // Projected closest point on segment B

  // Clamp projections
  if (t0 < 0)
    pA = a0;
  else if (t0 > magA)
    pA = a1;

  if (t1 < 0)
    pB = b0;
  else if (t1 > magB)
    pB = b1;

  float dot;
  // Clamp projection A
  if (t0 < 0 || t0 > magA) {
    dot = glm::dot(_B, (pA - b0));
    if (dot < 0)
      dot = 0;
    else if (dot > magB)
      dot = magB;
    pB = b0 + (_B * dot);
  }
  // Clamp projection B
  if (t1 < 0 || t1 > magB) {
    dot = glm::dot(_A, (pB - a0));
    if (dot < 0)
      dot = 0;
    else if (dot > magA)
      dot = magA;
    pA = a0 + (_A * dot);
  }

  result.A = pA;
  result.B = pB;
  result.dist = glm::length(pA - pB);
  return result;
}

// gets the closest point in triangle abc to p.
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

// checks if capsule and triangle overlaps.
bool CapsuleTriCheck(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, glm::vec3 R1,
                     glm::vec3 R2, float radius, float& dist) {
  int len = (glm::distance(R1, R2) / radius) + 1;
  for (int i = 0; i < len; i++) {
    glm::vec3 temp = R1 + (R2 - R1) * (float)i / (float)len;
    glm::vec3 close = closestPointTriangle(temp, P1, P2, P3);
    float tempdist = glm::distance(temp, close);
    if (tempdist < radius) {
      dist = tempdist;
      return true;
    }
  }
  return false;
}

// checks the angle of the slope.
bool Slopecheck(glm::vec3 normal) {
  normal = glm::normalize(normal);
  float angle = std::acosf(normal.z) * 180.f / (float)PI;
  if (angle < 0) angle *= -1;

  return angle > 135;
}

// checks if you collided with triangle while you moved.
// returns glm::vec3(0) if you haven't collided at all.
// returns the normal of collided triangle if you have.
glm::vec3 movecollisioncheck(glm::vec3 hitbox[], glm::vec3 checkposition,
                             float radius, int teamindex, float& dist) {
  glm::vec3 result = glm::vec3(0);
  for (int i = 0; i < Global->mapfaces.size(); i++) {
    float disttemp;
    if (CapsuleTriCheck(Global->Points[Global->mapfaces[i].points[0]].pos,
                        Global->Points[Global->mapfaces[i].points[1]].pos,
                        Global->Points[Global->mapfaces[i].points[2]].pos,
                        hitbox[0] + checkposition, hitbox[1] + checkposition,
                        radius, disttemp)) {
      glm::vec3 a = Global->Points[Global->mapfaces[i].points[2]].pos -
                    Global->Points[Global->mapfaces[i].points[0]].pos,
                b = Global->Points[Global->mapfaces[i].points[1]].pos -
                    Global->Points[Global->mapfaces[i].points[0]].pos;
      glm::vec3 temp = glm::cross(a, b);
      if (result == glm::vec3(0) || std::abs(temp.z) <= std::abs(result.z)) {
        result = temp;
        dist = disttemp;
      }
    }
  }
  for (int i = 0; i < Global->Entities.size(); i++) {
    Entity* tempentity = Global->Entities[i];
    if (tempentity->teamindex != teamindex) {
      raycheckresult temp =
          capsuleraycheck(hitbox[0] + checkposition, hitbox[1] + checkposition,
                          tempentity->hitbox[0] + tempentity->position,
                          tempentity->hitbox[1] + tempentity->position);
      glm::vec3 normal = glm::normalize(temp.B - temp.A);
      if (temp.dist < radius + tempentity->hitboxradius &&
          (result == glm::vec3(0) ||
           std::abs(normal.z) <= std::abs(result.z))) {
        result = normal;
        dist = temp.dist;
      }
    }
  }
  for (auto& i : Global->PlayerEntity) {
    Entity* tempentity = i.second;
    if (tempentity->teamindex != teamindex) {
      raycheckresult temp =
          capsuleraycheck(hitbox[0] + checkposition, hitbox[1] + checkposition,
                          tempentity->hitbox[0] + tempentity->position,
                          tempentity->hitbox[1] + tempentity->position);
      glm::vec3 normal = glm::normalize(temp.B - temp.A);
      if (temp.dist < radius + tempentity->hitboxradius &&
          (result == glm::vec3(0) ||
           std::abs(normal.z) <= std::abs(result.z))) {
        result = normal;
        dist = temp.dist;
      }
    }
  }
  return result;
}

// entity movement function.
// OPTIMIZE THIS LATER!! THIS IS HORRID.
// Also add more comments! This looks like nonsense to most people.
void EntityMove(Entity* tempentity) {
  // consider round trip time in deltatime.
  float dt = tempentity->deltatimelocal + deltaTime;
  tempentity->velocityvec3.z -= tempentity->gravity * dt;

  // set goal of movement.
  glm::vec3 tempmove =
      (glm::vec3({tempentity->velocityvec3.x + tempentity->movevec2.x,
                  tempentity->velocityvec3.y + tempentity->movevec2.y,
                  tempentity->velocityvec3.z}) *
       dt);
  glm::vec3 tempposition = tempentity->position,
            moveresult = glm::vec3({0, 0, 0});

  // set how much to loop.
  int temp = sqrtf(tempmove.x * tempmove.x + tempmove.y * tempmove.y) /
                 tempentity->hitboxradius * 2 +
             1;
  // get move distance of one loop.
  float dist =
      sqrtf(tempmove.x * tempmove.x + tempmove.y * tempmove.y) / float(temp);

  for (int i = 0; i < temp; i++) {
    tempposition.x += tempmove.x / (float)temp;
    tempposition.y += tempmove.y / (float)temp;
    float distfirst;
    glm::vec3 normal = movecollisioncheck(tempentity->hitbox, tempposition,
                                          tempentity->hitboxradius,
                                          tempentity->teamindex, distfirst);

    // didn't collide with anything while moving on x and y axis.
    if (normal == glm::vec3(0)) {
      moveresult.x += tempmove.x / (float)temp;
      moveresult.y += tempmove.y / (float)temp;
      // go down a little bit just in case you're on a downwards slope.
      for (int j = 1; j <= 16; j++) {
        float disttemp;
        glm::vec3 tempnormal = movecollisioncheck(
            tempentity->hitbox, tempposition - glm::vec3(0, 0, j * dist / 16.f),
            tempentity->hitboxradius, tempentity->teamindex, disttemp);
        if (tempnormal != glm::vec3(0)) {
          disttemp -= tempentity->hitboxradius;
          moveresult.z -= j * dist / 16.f + disttemp;
          tempposition.z -= j * dist / 16.f + disttemp;
          break;
        }
      }
    }
    // collided with something while moving on x and y axis.
    else {
      bool check = false;
      for (int j = 1; j <= 64; j++) {
        float disttemp;
        glm::vec3 tempnormal = movecollisioncheck(
            tempentity->hitbox, tempposition + glm::vec3(0, 0, j * dist / 64.f),
            tempentity->hitboxradius, tempentity->teamindex, disttemp);
        if (tempnormal == glm::vec3(0)) {
          moveresult.x += tempmove.x / (float)temp;
          moveresult.y += tempmove.y / (float)temp;
          moveresult.z += j * dist / 64.f;
          tempposition.z += j * dist / 64.f;
          // dist -= j * dist / 64.f;
          check = true;
          break;
        }
      }
      if (!check) {
        tempposition.x -= tempmove.x / (float)temp;
        tempposition.y -= tempmove.y / (float)temp;
        glm::vec3 tempmovexy = tempmove / (float)temp;
        tempmovexy.z = 0;
        normal.z = 0;
        normal = glm::cross(normal, glm::vec3({0, 0, 1}));
        glm::vec3 newmove =
            (glm::dot(tempmovexy, normal) / glm::dot(normal, normal)) * normal;

        if (normal == glm::vec3(0)) newmove = glm::vec3(0);

        tempposition += newmove;

        float disttemp;

        glm::vec3 tempnormal = movecollisioncheck(
            tempentity->hitbox, tempposition, tempentity->hitboxradius,
            tempentity->teamindex, disttemp);
        if (tempnormal == glm::vec3(0)) {
          moveresult += newmove;
        } else {
          check = false;
          for (int j = 1; j <= 64; j++) {
            float disttemp;
            glm::vec3 tempnormal = movecollisioncheck(
                tempentity->hitbox,
                tempposition + glm::vec3(0, 0, j * dist / 64.f),
                tempentity->hitboxradius, tempentity->teamindex, disttemp);
            if (tempnormal == glm::vec3(0)) {
              moveresult += newmove;
              moveresult.z += j * dist / 64.f;
              tempposition.z += j * dist / 64.f;
              // dist -= j * dist / 64.f;
              check = true;
              break;
            }
          }
          if (!check) break;
        }
      }
    }
  }

  tempposition = moveresult + tempentity->position;
  temp = (std::abs(tempmove.z) / tempentity->hitboxradius) * 4 + 1;
  for (int i = 0; i < temp; i++) {
    float disttempbase;
    tempposition.z += tempmove.z / (float)temp;
    glm::vec3 tempnormal = movecollisioncheck(
        tempentity->hitbox, tempposition, tempentity->hitboxradius,
        tempentity->teamindex, disttempbase);
    if (tempnormal == glm::vec3(0)) {
      tempentity->IsGrounded = false;
      moveresult.z += tempmove.z / (float)temp;
    } else {
      if (Slopecheck(tempnormal)) {
        tempentity->IsGrounded = true;
        tempentity->velocityvec3.z = -0.1f;
        break;
      } else {
        tempnormal.z = 0;
        tempnormal = glm::normalize(tempnormal);

        float dist = -(64.f * dt / (float)temp);
        int result = 0;

        float distthing = 0;
        for (int j = 1; j <= 16; j++) {
          tempposition.x += tempnormal.x * dist * j / 16.f;
          tempposition.y += tempnormal.y * dist * j / 16.f;
          float disttemp;
          if (movecollisioncheck(
                  tempentity->hitbox, tempposition, tempentity->hitboxradius,
                  tempentity->teamindex, disttemp) == glm::vec3(0)) {
            distthing = disttemp;
            result = j;
          }
          tempposition.x -= tempnormal.x * dist * j / 16.f;
          tempposition.y -= tempnormal.y * dist * j / 16.f;
          if (result > 0) break;
        }
        if (result == 0) {
          tempentity->IsGrounded = true;
          tempentity->velocityvec3.z = -0.1f;
          tempposition.z -= disttempbase;
          break;
        } else {
          tempentity->IsGrounded = false;
          moveresult.x += tempnormal.x * dist * result / 16.f;
          moveresult.y += tempnormal.y * dist * result / 16.f;
          // tempentity->velocityvec3.x += tempnormal.x * dist * result / 16.f;
          // tempentity->velocityvec3.y += tempnormal.y * dist * result / 16.f;
          tempposition.x += tempnormal.x * dist * result / 16.f;
          tempposition.y += tempnormal.y * dist * result / 16.f;
          moveresult.z += tempmove.z / (float)temp;
        }
      }
    }
  }

  tempentity->position = (tempentity->position + moveresult);
  glm::vec2 tempvec =
      glm::vec2({tempentity->velocityvec3.x, tempentity->velocityvec3.y});

  if (glm::length(tempvec) < 0.5f)
    tempvec = glm::vec2(0);
  else
    tempvec *= std::pow(0.001f, dt);

  tempentity->velocityvec3.x = tempvec.x;
  tempentity->velocityvec3.y = tempvec.y;
}
