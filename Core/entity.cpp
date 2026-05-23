#include "entity.h"

#include <cmath>

#include "extern.h"

void Entity::render() {
  switch (Settings->graphicsmode) {
    case 1: {
      float sinthing = std::sin((Camera->dir.x + 90) * PI / 180.0),
            costhing = std::cos((Camera->dir.x + 90) * PI / 180.0);

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,
                    Global->GLstuff->textures[billboardthing->sprite]);
      glBegin(GL_QUADS);
      glColor4f(1, 1, 1, 1);
      glTexCoord2f(billboardthing->uv[0].x, billboardthing->uv[0].y);
      glVertex3f(position.x - sinthing * billboardthing->size.x,
                 position.y - costhing * billboardthing->size.x / 2,
                 position.z + billboardthing->size.y);
      glTexCoord2f(billboardthing->uv[0].x + billboardthing->uv[1].x,
                   billboardthing->uv[0].y);
      glVertex3f(position.x + sinthing * billboardthing->size.x,
                 position.y + costhing * billboardthing->size.x / 2,
                 position.z + billboardthing->size.y);
      glTexCoord2f(billboardthing->uv[0].x + billboardthing->uv[1].x,
                   billboardthing->uv[0].y + billboardthing->uv[1].y);
      glVertex3f(position.x + sinthing * billboardthing->size.x,
                 position.y + costhing * billboardthing->size.x / 2,
                 position.z);
      glTexCoord2f(billboardthing->uv[0].x,
                   billboardthing->uv[0].y + billboardthing->uv[1].y);
      glVertex3f(position.x - sinthing * billboardthing->size.x / 2,
                 position.y - costhing * billboardthing->size.x / 2,
                 position.z);

      glEnd();
      break;
    }
    case 0: {
      // SDL_Surface* surface =
      //     Global->SRstuff->textures[billboardthing->sprite];
      // uv.first.x *= surface->w;
      // uv.first.y *= surface->h;
      // uv.second.x *= surface->w;
      // uv.second.y *= surface->h;
      // for (int i = 0; i < size.y; i++) {
      //   for (int j = 0; j < size.x; j++) {
      //     Uint8 color = static_cast<Uint8*>(surface->pixels)[int(
      //         uv.first.x + j + surface->pitch * (uv.first.y + i))];
      //     if (color > 0) {
      //       Global->SRstuff
      //           ->pixelsdepth[((int)realpos.x + j) +
      //                         ((int)realpos.y + i) *
      //                         Global->SRstuff->pitch] =
      //           0;
      //       Global->SRstuff
      //           ->pixels[((int)realpos.x + j) +
      //                    ((int)realpos.y + i) * Global->SRstuff->pitch] =
      //           color;
      //     }
      //   }
      // }
      break;
    }
  }
}