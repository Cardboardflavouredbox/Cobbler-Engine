#include "physics.h"

#include <SDL3/SDL_log.h>

#include <cmath>
#include <glm/glm.hpp>
#include <optional>

#include "deltaTime.h"
#include "extern.h"
#include "map.h"
#include "networkextern.h"
#include "pi.h"

// Ray and Triangle collision check function.
// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
std::optional<glm::vec3> RayTriIntersect(glm::vec3 ray_origin,
                                         glm::vec3 ray_vector, glm::vec3 tri1,
                                         glm::vec3 tri2, glm::vec3 tri3) {
  constexpr float epsilon = std::numeric_limits<float>::epsilon();

  glm::vec3 edge1 = tri2 - tri1;
  glm::vec3 edge2 = tri3 - tri1;

  // Backface culling, assuming CCW-wound triangles.
  const glm::vec3 normal = glm::cross(edge1, edge2);  // No need to normalize
  if (glm::dot(normal, ray_vector) > 0) return {};

  glm::vec3 ray_cross_e2 = glm::cross(ray_vector, edge2);
  float det = glm::dot(edge1, ray_cross_e2);

  if (std::abs(det) < epsilon) return {};  // Ray is parallel to triangle

  float inv_det = 1.0 / det;
  glm::vec3 s = ray_origin - tri1;
  float u = inv_det * glm::dot(s, ray_cross_e2);

  if (u < -epsilon || u - 1 > epsilon)
    return {};  // Ray passes outside edge2's bounds

  glm::vec3 s_cross_e1 = glm::cross(s, edge1);
  float v = inv_det * glm::dot(ray_vector, s_cross_e1);

  if (v < -epsilon || u + v - 1 > epsilon)
    return {};  // Ray passes outside edge1's bounds

  // The ray line intersects with the triangle.
  // We compute t to find where on the ray the intersection is.
  float t = inv_det * glm::dot(edge2, s_cross_e1);

  if (t > epsilon)  // Ray intersection
  {
    return glm::vec3(ray_origin + ray_vector * t);
  } else  // This means that there is a line intersection but not a ray
          // intersection.
    return {};
}

// checks if capsule and ray overlaps.
// pretty much compares two rays and see if the minimum distance is shorter than
// the radius.
// https://stackoverflow.com/a/18994296
// https://stackoverflow.com/questions/2824478/shortest-distance-between-two-line-segments#comment79231859_18994296
raycheckresult capsuleraycheck(glm::vec3 a0, glm::vec3 a1, glm::vec3 b0,
                               glm::vec3 b1) {
  raycheckresult result;

  if (a0 == a1) {
    float LineLength = glm::distance(b0, b1);
    glm::vec3 Vector = a0 - b0, LineDirection = (b1 - b0) / LineLength;

    // Project Vector to LineDirection to get the distance of point from a
    float Distance = glm::dot(Vector, LineDirection);

    result.A = a0;
    if (Distance <= 0)
      result.B = b0;
    else if (Distance >= LineLength)
      result.B = b1;
    else {
      result.B = b0 + LineDirection * Distance;
    }

    result.dist = glm::distance(result.A, result.B);

    return result;
  }

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
                     glm::vec3 R2, float radius, float& dist,
                     glm::vec3& normal) {
  float resultdist = 0;
  glm::vec3 normalresult;
  int len = (glm::distance(R1, R2) / radius) + 1;
  for (int i = 0; i < len; i++) {
    glm::vec3 temp = R1 + (R2 - R1) * (float)i / (float)len;
    glm::vec3 close = closestPointTriangle(temp, P1, P2, P3);
    float tempdist = glm::distance(temp, close);
    if (tempdist < radius && (resultdist == 0 || resultdist > tempdist)) {
      normalresult = glm::normalize(close - temp);
      resultdist = tempdist;
    }
  }
  dist = resultdist;
  normal = normalresult;
  return (resultdist > 0);
}

// checks the angle of the slope.
float Slopecheck(glm::vec3 normal) {
  normal = glm::normalize(normal);

  return std::acosf(normal.z) * 180.f / (float)PI;
}

// checks if you collided with triangle while you moved.
// returns glm::vec3(0) if you haven't collided at all.
// returns the normal of collided triangle if you have.
glm::vec3 movecollisioncheck(glm::vec3 hitbox[], glm::vec3 checkposition,
                             float radius, int teamindex,
                             movecollisionresult& resultinfo,
                             Entity* movingentity) {
  if (movingentity != NULL) {
    for (int i = 0; i < GlobalMapStuff->KillboxFaces.size(); i++) {
      float disttemp;
      glm::vec3 normal;
      if (CapsuleTriCheck(
              GlobalMapStuff->KillboxPoints[GlobalMapStuff->KillboxFaces[i][0]],
              GlobalMapStuff->KillboxPoints[GlobalMapStuff->KillboxFaces[i][1]],
              GlobalMapStuff->KillboxPoints[GlobalMapStuff->KillboxFaces[i][2]],
              hitbox[0] + checkposition, hitbox[1] + checkposition, radius,
              disttemp, normal)) {
        movingentity->hp = -1;
      }
    }
  }

  movecollisionresult resultthing;
  glm::vec3 result = glm::vec3(0);
  for (int i = 0; i < GlobalMapStuff->Hitboxmapfaces.size(); i++) {
    float disttemp;
    glm::vec3 normal;
    if (CapsuleTriCheck(
            GlobalMapStuff->HitboxPoints[GlobalMapStuff->Hitboxmapfaces[i][0]],
            GlobalMapStuff->HitboxPoints[GlobalMapStuff->Hitboxmapfaces[i][1]],
            GlobalMapStuff->HitboxPoints[GlobalMapStuff->Hitboxmapfaces[i][2]],
            hitbox[0] + checkposition, hitbox[1] + checkposition, radius,
            disttemp, normal)) {
      if (resultthing.dist == 0 || resultthing.dist > disttemp) {
        result = normal;
        resultthing.dist = disttemp;
        resultthing.CollidedWithPlayer = false;
        resultthing.CollidedwithEntityAtAll = false;
      }
    }
  }
  for (auto& i : Entities) {
    Entity* tempentity = i.second;
    if (tempentity->teamindex != teamindex) {
      raycheckresult temp =
          capsuleraycheck(hitbox[0] + checkposition, hitbox[1] + checkposition,
                          tempentity->hitbox[0] + tempentity->position,
                          tempentity->hitbox[1] + tempentity->position);
      glm::vec3 normal = glm::normalize(temp.B - temp.A);
      if (temp.dist < radius + tempentity->hitboxradius &&
          (resultthing.dist == 0 ||
           resultthing.dist > -temp.dist + tempentity->hitboxradius)) {
        result = normal;
        resultthing.dist = -temp.dist + tempentity->hitboxradius;
        resultthing.CollidedWithPlayer = false;
        resultthing.CollidedwithEntityAtAll = true;
        resultthing.collidedID = i.first;
      }
    }
  }
  for (auto& i : GlobalNetworkStuff->PlayerNetStuff) {
    Entity* tempentity = i.second.PlayerEntity;
    if (tempentity->teamindex != teamindex) {
      raycheckresult temp =
          capsuleraycheck(hitbox[0] + checkposition, hitbox[1] + checkposition,
                          tempentity->hitbox[0] + tempentity->position,
                          tempentity->hitbox[1] + tempentity->position);
      glm::vec3 normal = glm::normalize(temp.B - temp.A);
      if (temp.dist < radius + tempentity->hitboxradius &&
          (resultthing.dist == 0 ||
           resultthing.dist > -temp.dist + tempentity->hitboxradius)) {
        result = normal;
        resultthing.dist = -temp.dist + tempentity->hitboxradius;
        resultthing.CollidedWithPlayer = true;
        resultthing.CollidedwithEntityAtAll = true;
        resultthing.collidedID = i.first;
      }
    }
  }
  resultinfo = resultthing;
  return result;
}

void raycastcheck(glm::vec3 hitbox[], int teamindex,
                  movecollisionresult& resultinfo) {
  movecollisionresult resultthing;
  resultthing.dist = 0;
  for (auto& i : GlobalMapStuff->Hitboxmapfaces) {
    std::optional<glm::vec3> check = RayTriIntersect(
        hitbox[0], glm::normalize(hitbox[1] - hitbox[0]),
        GlobalMapStuff->HitboxPoints[i[0]], GlobalMapStuff->HitboxPoints[i[1]],
        GlobalMapStuff->HitboxPoints[i[2]]);
    if (check.has_value()) {
      float disttemp = glm::distance(hitbox[0], check.value());
      SDL_Log("1 %f", disttemp);
      if (resultthing.dist == 0 || resultthing.dist > disttemp) {
        resultthing.dist = disttemp;
        resultthing.CollidedWithPlayer = false;
        resultthing.CollidedwithEntityAtAll = false;
      }
    }
  }
  for (auto& i : Entities) {
    Entity* tempentity = i.second;
    if (tempentity->teamindex != teamindex) {
      raycheckresult temp = capsuleraycheck(
          hitbox[0], hitbox[1], tempentity->hitbox[0] + tempentity->position,
          tempentity->hitbox[1] + tempentity->position);

      float disttemp = glm::distance(temp.A, hitbox[0]);

      if (temp.dist < tempentity->hitboxradius &&
          (resultthing.dist == 0 || resultthing.dist > disttemp)) {
        SDL_Log("2 %f", disttemp);
        resultthing.dist = disttemp;
        resultthing.CollidedWithPlayer = false;
        resultthing.CollidedwithEntityAtAll = true;
        resultthing.collidedID = i.first;
      }
    }
  }
  for (auto& i : GlobalNetworkStuff->PlayerNetStuff) {
    Entity* tempentity = i.second.PlayerEntity;
    if (tempentity->teamindex != teamindex) {
      raycheckresult temp = capsuleraycheck(
          hitbox[0], hitbox[1], tempentity->hitbox[0] + tempentity->position,
          tempentity->hitbox[1] + tempentity->position);

      float disttemp = glm::distance(temp.A, hitbox[0]);

      if (temp.dist < tempentity->hitboxradius &&
          (resultthing.dist == 0 || resultthing.dist > disttemp)) {
        SDL_Log("3 %f", disttemp);
        resultthing.dist = disttemp;
        resultthing.CollidedWithPlayer = true;
        resultthing.CollidedwithEntityAtAll = true;
        resultthing.collidedID = i.first;
      }
    }
  }
  resultinfo = resultthing;
}

glm::vec3 collisionloop(Entity* tempentity, glm::vec3 tempposition) {
  movecollisionresult resultthing;
  glm::vec3 normal = movecollisioncheck(
      tempentity->hitbox, tempposition, tempentity->hitboxradius,
      tempentity->teamindex, resultthing, tempentity);

  float dist = -resultthing.dist + tempentity->hitboxradius;
  // SDL_Log("%f", distfirst);
  glm::vec3 newmove = dist * glm::normalize(normal);

  if (normal == glm::vec3(0)) newmove = glm::vec3(0);

  return -newmove;
}

// entity movement function.
// OPTIMIZE THIS LATER!! THIS IS HORRID.
// Also add more comments! This looks like nonsense to most people.
void EntityMove(Entity* tempentity) {
  // consider round trip time in deltatime.
  float dt = tempentity->deltatimelocal + updatedeltaTime;
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
    movecollisionresult resultthing;
    glm::vec3 normal = movecollisioncheck(
        tempentity->hitbox, tempposition, tempentity->hitboxradius,
        tempentity->teamindex, resultthing, tempentity);
    float distfirst = resultthing.dist;

    // didn't collide with anything while moving on x and y axis.
    if (normal == glm::vec3(0)) {
      tempentity->Collided = false;
      moveresult.x += tempmove.x / (float)temp;
      moveresult.y += tempmove.y / (float)temp;
      // go down a little bit just in case you're on a downwards slope.
      if (tempentity->gravity != 0) {
        for (int j = 1; j <= 16; j++) {
          movecollisionresult resultthing;
          glm::vec3 tempnormal = movecollisioncheck(
              tempentity->hitbox,
              tempposition - glm::vec3(0, 0, j * dist / 16.f),
              tempentity->hitboxradius, tempentity->teamindex, resultthing,
              tempentity);
          float disttemp = resultthing.dist;
          if (tempnormal != glm::vec3(0)) {
            disttemp -= tempentity->hitboxradius;
            moveresult.z -= j * dist / 16.f + disttemp;
            tempposition.z -= j * dist / 16.f + disttemp;
            break;
          }
        }
      }
    }
    // collided with something while moving on x and y axis.
    else {
      bool check = false;
      float disttempcache = 0;
      tempentity->Collided = true;
      if (tempentity->gravity != 0) {
        // try moving up just in case it's an upwards slope.
        for (int j = 1; j <= 16; j++) {
          movecollisionresult resultthing;
          glm::vec3 tempnormal = movecollisioncheck(
              tempentity->hitbox,
              tempposition + glm::vec3(0, 0, j * dist / 16.f),
              tempentity->hitboxradius, tempentity->teamindex, resultthing,
              tempentity);
          float disttemp = resultthing.dist;
          if (tempnormal == glm::vec3(0)) {
            if (disttempcache != 0)
              disttempcache = tempentity->hitboxradius - disttempcache;
            moveresult.x += tempmove.x / (float)temp;
            moveresult.y += tempmove.y / (float)temp;
            moveresult.z += (j - 1) * dist / 16.f - disttempcache;
            tempposition.z += (j - 1) * dist / 16.f - disttempcache;
            // dist -= (j-1) * dist / 16.f;
            check = true;
            break;
          }
          disttempcache = disttemp;
        }
      }
      if (!check) {
        moveresult.x += tempmove.x / (float)temp;
        moveresult.y += tempmove.y / (float)temp;
        // normal.z = 0;
        distfirst = -distfirst + tempentity->hitboxradius;
        // SDL_Log("%f", distfirst);
        glm::vec3 newmove = distfirst * glm::normalize(normal);

        if (normal == glm::vec3(0)) newmove = glm::vec3(0);

        tempposition -= newmove;
        moveresult -= newmove;

        glm::vec3 tempvec3(1);
        while (glm::length(tempvec3) > 0.00001) {
          tempvec3 = collisionloop(tempentity, tempposition);

          tempposition += tempvec3;
          moveresult += tempvec3;
        }
        // break;
      }
    }
  }

  tempposition = moveresult + tempentity->position;
  temp = (std::abs(tempmove.z) / tempentity->hitboxradius) * 4 + 1;
  for (int i = 0; i < temp; i++) {
    tempposition.z += tempmove.z / (float)temp;
    movecollisionresult resultthing;
    glm::vec3 tempnormal = movecollisioncheck(
        tempentity->hitbox, tempposition, tempentity->hitboxradius,
        tempentity->teamindex, resultthing, tempentity);
    float disttempbase = resultthing.dist;
    if (tempnormal == glm::vec3(0)) {
      tempentity->IsGrounded = false;
      moveresult.z += tempmove.z / (float)temp;
    } else {
      float anglething = Slopecheck(tempnormal);
      if (anglething < 0) anglething *= -1;
      if (anglething > 135) {
        tempentity->IsGrounded = true;
        tempentity->velocityvec3.z = -0.1f;
        break;
      } else if (anglething < 45) {
        tempentity->IsGrounded = false;
        tempentity->Collided = true;
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
          movecollisionresult resultthing;
          if (movecollisioncheck(tempentity->hitbox, tempposition,
                                 tempentity->hitboxradius,
                                 tempentity->teamindex, resultthing,
                                 tempentity) == glm::vec3(0)) {
            distthing = resultthing.dist;
            result = j;
          }
          tempposition.x -= tempnormal.x * dist * j / 16.f;
          tempposition.y -= tempnormal.y * dist * j / 16.f;
          if (result > 0) break;
        }
        if (result == 0) {
          tempentity->IsGrounded = true;
          tempentity->velocityvec3.z = -0.1f;
          tempposition.z -= tempentity->hitboxradius - disttempbase;
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

  if (glm::length(tempvec) < 0.0001f)
    tempvec = glm::vec2(0);
  else
    tempvec *= std::pow(tempentity->IsGrounded ? 0.001f : 0.125f, dt);

  tempentity->velocityvec3.x = tempvec.x;
  tempentity->velocityvec3.y = tempvec.y;

  if (tempentity->IsGrounded) tempentity->Collided = true;
}
