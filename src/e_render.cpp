#include <cmath>

#include "extern.h"
#include "map.h"
#include "rendermath.h"
#include "update.h"

glm::vec2 ToScreenSpace(glm::vec3 P) {
  glm::vec2 temp = (glm::vec2)P - Editor->pos;
  temp *= Editor->zoom;
  temp.x *= -1;
  temp.x += (Settings->resolutionx / 2);
  temp.y += (Settings->resolutiony / 2);
  return temp;
}

bool pointoffscreen(glm::vec3 P) { return false; }

void DrawTri(Mapface face) {
  glm::vec2 vectors[3] = {ToScreenSpace(Global->Points[face.points[0]]),
                          ToScreenSpace(Global->Points[face.points[1]]),
                          ToScreenSpace(Global->Points[face.points[2]])};
  int x = vectors[0].x, x2 = vectors[0].x, y = vectors[0].y, y2 = vectors[0].y;
  for (int i = 1; i < 3; i++) {
    if (vectors[i].x < x) x = vectors[i].x;
    if (vectors[i].x > x2) x2 = vectors[i].x;
    if (vectors[i].y < y) y = vectors[i].y;
    if (vectors[i].y > y2) y2 = vectors[i].y;
  }
  if (x < 0) x = 0;
  if (x >= Settings->resolutionx - 1) x = Settings->resolutionx - 1;
  if (x2 < 0) x2 = 0;
  if (x2 >= Settings->resolutionx - 1) x2 = Settings->resolutionx - 1;
  if (y < 0) y = 0;
  if (y >= Settings->resolutiony - 1) y = Settings->resolutiony - 1;
  if (y2 < 0) y2 = 0;
  if (y2 >= Settings->resolutiony - 1) y2 = Settings->resolutiony - 1;
  for (int i = x; i <= x2; i++) {
    for (int j = y; j <= y2; j++) {
      glm::vec2 temp;
      temp.x = i;
      temp.y = j;
      if (temp.x >= 0 && temp.y >= 0 && temp.x < Settings->resolutionx &&
          temp.y < Settings->resolutiony) {
        if (Vec2inTri(temp, vectors[0], vectors[1], vectors[2])) {
          glm::vec3 uvw = GetUV(temp, vectors[0], vectors[1], vectors[2]);
          glm::vec2 uvresult =
              ((((face.UVs[0] * uvw.x) * Global->Points[face.points[0]].z) +
                ((face.UVs[1] * uvw.y) * Global->Points[face.points[1]].z)) +
               ((face.UVs[2] * uvw.z) * Global->Points[face.points[2]].z));
          uvresult =
              (uvresult * (1 / (uvw.x * Global->Points[face.points[0]].z +
                                uvw.y * Global->Points[face.points[1]].z +
                                uvw.z * Global->Points[face.points[2]].z)));

          int uvxthing = (int(128 * (uvresult.x)) * face.xloop) % 128;
          int uvything = (int(128 * (uvresult.y)) * face.yloop) % 128;

          glm::vec3 tempvec3;
          tempvec3.x = Global->Points[face.points[0]].x * uvw.x +
                       Global->Points[face.points[1]].x * uvw.y +
                       Global->Points[face.points[2]].x * uvw.z;
          tempvec3.y = Global->Points[face.points[0]].y * uvw.x +
                       Global->Points[face.points[1]].y * uvw.y +
                       Global->Points[face.points[2]].y * uvw.z;
          tempvec3.z = Global->Points[face.points[0]].z;
          tempvec3.x -= Editor->pos.x;
          tempvec3.y -= Editor->pos.y;
          float dist =
              std::sqrt(tempvec3.x * tempvec3.x + tempvec3.y * tempvec3.y +
                        tempvec3.z * tempvec3.z);
          if (Global->pixelsdepth[i + j * Global->pitch] > dist * 3 /*||
              Global->pixelstransparency[i + j * Global->pitch] < 255*/) {
            // if (Global->pixelstransparency[i + j * Global->pitch] < 255) {
            //   int transparency =
            //       Global->pixelstransparency[i + j * Global->pitch];
            //   SDL_Color tempcolor =
            //       Global->palette
            //           ->colors[Global->pixels[i + j * Global->pitch]];
            //   r = r * (255 - transparency) / 255 +
            //       tempcolor.r * transparency / 255;
            //   g = g * (255 - transparency) / 255 +
            //       tempcolor.g * transparency / 255;
            //   b = b * (255 - transparency) / 255 +
            //       tempcolor.b * transparency / 255;
            // }
            // if (r < 0) r = 0;
            // if (g < 0) g = 0;
            // if (b < 0) b = 0;

            Global->pixels[i + j * Global->pitch] =
                static_cast<Uint8*>(Global->textures[face.texture]
                                        ->pixels)[uvxthing + uvything * 128];
            if (dist < 0) dist = 0;
            if (Global->pixelstransparency[i + j * Global->pitch] == 255)
              Global->pixelsdepth[i + j * Global->pitch] =
                  (unsigned char)dist * 4;
            Global->pixelstransparency[i + j * Global->pitch] = 255;
          }
        }
      }
    }
  }
}

void rendergame() {
  if (Global->rendermode == 0) {
    std::deque<Mapface> tempmapfacedeque = Global->mapfaces;
    for (int i = 0; i < tempmapfacedeque.size(); i++) {
      Mapface* tempmapface = &tempmapfacedeque[i];
      int check = 0;
      for (int j = 0; j < 3; j++) {
        if (!pointoffscreen(Global->Points[tempmapface->points[j]])) break;
        check++;
      }
      if (check == 3) {
        tempmapfacedeque.erase(tempmapfacedeque.begin() + i);
        i--;
      } else
        DrawTri(*tempmapface);
    }
  }
}

void renderUI() {
  std::deque<UIthing*>* tempdeque = &Global->UImap[Global->UIindex];
  int len = tempdeque->size();
  for (int i = 0; i < len; i++) {
    tempdeque->at(i)->render();
  }
}

void render() {
  SDL_LockSurface(Global->render_target);

  Global->pixels = static_cast<unsigned char*>(Global->render_target->pixels);
  Global->pitch = Global->render_target->pitch;

  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      Global->pixelsdepth[i + j * Global->pitch] = 65535;
      Global->pixelstransparency[i + j * Global->pitch] = 255;
    }
  }
  renderUI();
  rendergame();

  unsigned char bgcolor = Global->rendermode == 1 ? 1 : 0;
  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      if (Global->pixelsdepth[i + j * Global->pitch] == 65535) {
        Global->pixels[i + j * Global->pitch] = bgcolor;
      }
    }
  }

  if (Global->pause || Global->isopeningfile) {
    for (int i = 0; i < Settings->resolutionx; i++) {
      for (int j = 0; j < Settings->resolutiony; j++) {
        Uint8 r, g, b;
        SDL_GetRGB(Global->pixels[i + j * Global->pitch],
                   SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_INDEX8),
                   Global->palette, &r, &g, &b);
        int r2 = r, g2 = g, b2 = b;
        r2 -= 64;
        g2 -= 64;
        b2 -= 64;
        if (r2 < 0) r2 = 0;
        if (g2 < 0) g2 = 0;
        if (b2 < 0) b2 = 0;
        Global->pixels[i + j * Global->pitch] =
            SDL_MapSurfaceRGB(Global->render_target, r2, g2, b2);
      }
    }
  }

  SDL_UnlockSurface(Global->render_target);

  // Screen size and position stuff
  int w = Global->windowx, h = Global->windowy, rtw = Global->render_target->w,
      rth = Global->render_target->h;
  int size = w / rtw;
  if (size > h / rth) size = h / rth;

  rtw *= size;
  rth *= size;

  w /= 2;
  h /= 2;
  w -= rtw / 2;
  h -= rth / 2;

  SDL_FRect temprect;
  temprect.w = rtw;
  temprect.h = rth;
  temprect.x = w;
  temprect.y = h;
  SDL_Texture* temptexture =
      SDL_CreateTextureFromSurface(Global->renderer, Global->render_target);
  SDL_SetTextureScaleMode(temptexture, SDL_SCALEMODE_PIXELART);
  SDL_RenderTexture(Global->renderer, temptexture, NULL, &temprect);
  SDL_RenderPresent(Global->renderer);
}