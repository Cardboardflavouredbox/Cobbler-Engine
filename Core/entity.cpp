#include "entity.h"

#include <SDL3/SDL_log.h>

#include <cmath>

#include "extern.h"
#include "model.h"
#include "render.h"

int GetBillBoardIndex(float angle, int lastIndex) {
  // front
  if (angle > -22.5f && angle < 22.6f) return 0;
  if (angle >= 22.5f && angle < 67.5f) return 7;
  if (angle >= 67.5f && angle < 112.5f) return 6;
  if (angle >= 112.5f && angle < 157.5f) return 5;

  // back
  if (angle <= -157.5 || angle >= 157.5f) return 4;
  if (angle >= -157.4f && angle < -112.5f) return 3;
  if (angle >= -112.5f && angle < -67.5f) return 2;
  if (angle >= -67.5f && angle <= -22.5f) return 1;

  return lastIndex;
}
void Entity::rendermodelgroup() {
  renderModelGroup(*Modelthing, &ModelGroupMap[Modelthing->name], false);
}

void Entity::renderbillboard() {
  float sinthing = std::sin((-Camera->dir.x + 90) * PI / 180.0),
        costhing = std::cos((-Camera->dir.x + 90) * PI / 180.0);
  glm::vec3 points[4] = {
      {position.x - sinthing * (billboardthing->size.x / 2),
       position.y - costhing * billboardthing->size.x / 2,
       position.z + billboardthing->size.y + billboardthing->offset},
      {position.x + sinthing * billboardthing->size.x / 2,
       position.y + costhing * billboardthing->size.x / 2,
       position.z + billboardthing->size.y + billboardthing->offset},
      {position.x + sinthing * billboardthing->size.x / 2,
       position.y + costhing * billboardthing->size.x / 2,
       position.z + billboardthing->offset},
      {position.x - sinthing * billboardthing->size.x / 2,
       position.y - costhing * billboardthing->size.x / 2,
       position.z + billboardthing->offset}};
  switch (Settings->graphicsmode) {
    case 1: {
      for (int i = 0; i < 4; i++) points[i] -= Camera->position;
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,
                    Global->GLstuff->textures[billboardthing->sprite]);
      glBegin(GL_QUADS);
      glColor4f(1, 1, 1, 1);
      glTexCoord2f(billboardthing->uv[0].x, billboardthing->uv[0].y);
      glVertex3f(points[0].x, points[0].y, points[0].z);
      glTexCoord2f(billboardthing->uv[0].x + billboardthing->uv[1].x,
                   billboardthing->uv[0].y);
      glVertex3f(points[1].x, points[1].y, points[1].z);
      glTexCoord2f(billboardthing->uv[0].x + billboardthing->uv[1].x,
                   billboardthing->uv[0].y + billboardthing->uv[1].y);
      glVertex3f(points[2].x, points[2].y, points[2].z);
      glTexCoord2f(billboardthing->uv[0].x,
                   billboardthing->uv[0].y + billboardthing->uv[1].y);
      glVertex3f(points[3].x, points[3].y, points[3].z);

      glEnd();
      break;
    }
    default: {
      glm::vec3 temp[3] = {points[0], points[1], points[2]};
      glm::vec2 temp2[3] = {
          billboardthing->uv[0],
          glm::vec2({billboardthing->uv[0].x + billboardthing->uv[1].x,
                     billboardthing->uv[0].y}),
          billboardthing->uv[0] + billboardthing->uv[1]};
      DrawTri(billboardthing->sprite, temp, temp2, 1, 1);
      temp[0] = points[2];
      temp[1] = points[3];
      temp[2] = points[0];
      temp2[0] = billboardthing->uv[0] + billboardthing->uv[1];
      temp2[1] = glm::vec2({billboardthing->uv[0].x,
                            billboardthing->uv[0].y + billboardthing->uv[1].y});
      temp2[2] = billboardthing->uv[0];
      DrawTri(billboardthing->sprite, temp, temp2, 1, 1);
      break;
    }
  }
}