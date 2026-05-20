#include <SDL3/SDL_log.h>

#include "entityphysics.h"

glm::vec2 SAT3Dto2D(int normal, glm::vec3 point) {
  switch (normal) {
    case 0: {  //(1,0,0)
      return glm::vec2({point.y, point.z});
      break;
    }
    case 1: {  //(0,1,0)
      return glm::vec2({point.x, point.z});
      break;
    }
    case 2: {  //(0,0,1)
      return glm::vec2({point.x, point.y});
      break;
    }
  }
  return glm::vec2({0, 0});
}

float SAT2Dtofloat(int normal, glm::vec2 point) {
  if (normal == 0)
    return point.x;
  else
    return point.y;
}

bool SATcubetri(glm::vec3 aabb[], glm::vec3 tri[]) {
  for (int i = 0; i < 3; i++) {
    glm::vec2 tempsquare[2] = {SAT3Dto2D(i, aabb[0]), SAT3Dto2D(i, aabb[1])},
              temptriangle[3] = {SAT3Dto2D(i, tri[0]), SAT3Dto2D(i, tri[1]),
                                 SAT3Dto2D(i, tri[2])};
    for (int j = 0; j < 2; j++) {
      float square[2] = {SAT2Dtofloat(j, tempsquare[0]),
                         SAT2Dtofloat(j, tempsquare[1])},
            triangle[2] = {SAT2Dtofloat(j, temptriangle[0]),
                           SAT2Dtofloat(j, temptriangle[1])};
      if (square[0] > square[1]) {
        float temp = square[0];
        square[0] = square[1];
        square[1] = temp;
      }

      if (triangle[0] > triangle[1]) {
        float temp = triangle[0];
        triangle[0] = triangle[1];
        triangle[1] = temp;
      }

      float trithing = SAT2Dtofloat(j, temptriangle[2]);
      if (trithing < triangle[0])
        triangle[0] = trithing;
      else if (trithing > triangle[1])
        triangle[1] = trithing;

      if (square[0] >= triangle[1] || square[1] <= triangle[0]) return false;
    }
  }
  return true;
}

bool movecollisioncheck(glm::vec3 hitbox[], glm::vec3 startposition,
                        glm::vec3 endposition) {  // true if collided

  for (int i = 0; i < Global->mapfaces.size(); i++) {
    glm::vec3 aabb[2];
    if (startposition.x < endposition.x || startposition.y < endposition.y ||
        startposition.z < endposition.z) {
      aabb[0] = (startposition + hitbox[0]);
      aabb[1] = (endposition + hitbox[1]);
    } else {
      aabb[0] = (endposition + hitbox[0]);
      aabb[1] = (startposition + hitbox[1]);
    }
    glm::vec3 triangle[3] = {Global->Points[Global->mapfaces[i].points[0]],
                             Global->Points[Global->mapfaces[i].points[1]],
                             Global->Points[Global->mapfaces[i].points[2]]};
    if (SATcubetri(aabb, triangle)) return true;
  }
  return false;
}

void EntityMove(std::shared_ptr<Entity> tempentity) {
  tempentity->velocityvec3.z -=
      tempentity->gravity * (Global->deltaTime / 10.f);

  glm::vec3 tempmove = ((tempentity->movevec3 + tempentity->velocityvec3) *
                        (Global->deltaTime / 10.f));
  glm::vec3 tempposition = tempentity->position,
            moveresult = glm::vec3({0, 0, 0});
  tempposition.x += tempmove.x;
  if (!movecollisioncheck(tempentity->hitbox, tempentity->position,
                          tempposition)) {
    moveresult.x += tempmove.x;
  }

  tempposition = tempentity->position;
  tempposition.y += tempmove.y;
  if (!movecollisioncheck(tempentity->hitbox, tempentity->position,
                          tempposition)) {
    moveresult.y += tempmove.y;
  }

  tempposition = tempentity->position;
  tempposition.z += tempmove.z;
  if (!movecollisioncheck(tempentity->hitbox, tempentity->position,
                          tempposition)) {
    tempentity->IsGrounded = false;
    moveresult.z += tempmove.z;
  } else {
    tempentity->IsGrounded = true;
    tempentity->velocityvec3.z = -2.f;
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