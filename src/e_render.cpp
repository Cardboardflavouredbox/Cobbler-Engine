#include <cmath>
#include <glm/gtc/type_ptr.hpp>

#include "extern.h"
#include "map.h"
#include "rendermath.h"
#include "screen.h"
#include "update.h"

bool pointoffscreen(glm::vec3 P) { return false; }

void DrawCircle(unsigned char color, glm::vec3 rawpoint, int radius) {
  ScreenPoint point = ToScreenSpace(rawpoint);
  if (!point.isbehindcamera)
    for (int i = point.p.x - radius; i <= point.p.x + radius; i++) {
      for (int j = point.p.y - radius; j < point.p.y + radius; j++) {
        if (i > -1 && i < Settings->resolutionx && j > -1 &&
            j < Settings->resolutiony) {
          if (Global->SRstuff->pixelsdepth[i + j * Global->SRstuff->pitch] >=
              7) {
            Global->SRstuff->pixelsdepth[i + j * Global->SRstuff->pitch] = 7;
            Global->SRstuff->pixels[i + j * Global->SRstuff->pitch] = color;
          }
        }
      }
    }
}

void DrawTri(Mapface face) {
  glm::vec2 vectors[3] = {ToScreenSpace(Global->Points[face.points[0]]).p,
                          ToScreenSpace(Global->Points[face.points[1]]).p,
                          ToScreenSpace(Global->Points[face.points[2]]).p};
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
          if (Global->SRstuff->pixelsdepth[i + j * Global->SRstuff->pitch] >
              dist * 3 /*||
Global->SRstuff->pixelstransparency[i + j * Global->SRstuff->pitch] <
255*/) {
            // if (Global->SRstuff->pixelstransparency[i + j *
            // Global->SRstuff->pitch] < 255) {
            //   int transparency =
            //       Global->SRstuff->pixelstransparency[i + j *
            //       Global->SRstuff->pitch];
            //   SDL_Color tempcolor =
            //       Global->palette
            //           ->colors[Global->SRstuff->pixels[i + j *
            //           Global->SRstuff->pitch]];
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

            Global->SRstuff->pixels[i + j * Global->SRstuff->pitch] =
                static_cast<Uint8*>(Global->SRstuff->textures[face.texture]
                                        ->pixels)[uvxthing + uvything * 128];
            if (dist < 0) dist = 0;
            if (Global->SRstuff
                    ->pixelstransparency[i + j * Global->SRstuff->pitch] == 255)
              Global->SRstuff->pixelsdepth[i + j * Global->SRstuff->pitch] =
                  (unsigned char)dist * 4;
            Global->SRstuff
                ->pixelstransparency[i + j * Global->SRstuff->pitch] = 255;
          }
        }
      }
    }
  }
}

void rendergame() {
  if (Editor->UIindex == 0) {
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
    for (int i = 0; i < Global->Points.size(); i++) {
      DrawCircle(8, Global->Points[i], 2);
    }
  }
}

void renderGrid() {
  if (Editor->zoom >= 1) {
    int cnt = Settings->resolutionx / Editor->zoom;
    for (int i = -cnt - Editor->pos.x; i < cnt - Editor->pos.x; i++) {
      float pos = i + Editor->pos.x;
      pos *= Editor->zoom;
      pos += Settings->resolutionx / 2;
      if (0 <= pos && pos < Settings->resolutionx) {
        for (int j = 0; j < Settings->resolutiony; j++) {
          int index = (int)pos + j * Global->SRstuff->pitch;
          if (Global->SRstuff->pixelsdepth[index] > 0) {
            Global->SRstuff->pixels[index] = 11;
            Global->SRstuff->pixelsdepth[index] = 0;
            Global->SRstuff->pixelstransparency[index] = 255 / 3 * 2;
          }
        }
      }
    }

    cnt = Settings->resolutiony / Editor->zoom;
    for (int i = -cnt - Editor->pos.y; i < cnt - Editor->pos.y; i++) {
      float pos = i + Editor->pos.y;
      pos *= Editor->zoom;
      pos += Settings->resolutiony / 2;
      if (0 <= pos && pos < Settings->resolutiony) {
        for (int j = 0; j < Settings->resolutionx; j++) {
          int index =
              j + int(Settings->resolutiony - pos) * Global->SRstuff->pitch;
          if (Global->SRstuff->pixelsdepth[index] > 0) {
            Global->SRstuff->pixels[index] = 11;
            Global->SRstuff->pixelsdepth[index] = 0;
            Global->SRstuff->pixelstransparency[index] = 255 / 3 * 2;
          }
        }
      }
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

void softwarerender() {
  SDL_LockSurface(Global->SRstuff->render_target);

  Global->SRstuff->pixels =
      static_cast<unsigned char*>(Global->SRstuff->render_target->pixels);
  Global->SRstuff->pitch = Global->SRstuff->render_target->pitch;

  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      Global->SRstuff->pixelsdepth[i + j * Global->SRstuff->pitch] = 65535;
      Global->SRstuff->pixelstransparency[i + j * Global->SRstuff->pitch] = 255;
    }
  }
  renderUI();
  renderGrid();
  rendergame();

  unsigned char bgcolor = Editor->UIindex == 1 ? 1 : 0;
  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      if (Global->SRstuff->pixelsdepth[i + j * Global->SRstuff->pitch] ==
          65535) {
        Global->SRstuff->pixels[i + j * Global->SRstuff->pitch] = bgcolor;
      }
    }
  }

  if (Global->pause || Global->isopeningfile) {
    for (int i = 0; i < Settings->resolutionx; i++) {
      for (int j = 0; j < Settings->resolutiony; j++) {
        Uint8 r, g, b;
        SDL_GetRGB(Global->SRstuff->pixels[i + j * Global->SRstuff->pitch],
                   SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_INDEX8),
                   Global->SRstuff->palette, &r, &g, &b);
        int r2 = r, g2 = g, b2 = b;
        r2 -= 64;
        g2 -= 64;
        b2 -= 64;
        if (r2 < 0) r2 = 0;
        if (g2 < 0) g2 = 0;
        if (b2 < 0) b2 = 0;
        Global->SRstuff->pixels[i + j * Global->SRstuff->pitch] =
            SDL_MapSurfaceRGB(Global->SRstuff->render_target, r2, g2, b2);
      }
    }
  }

  SDL_UnlockSurface(Global->SRstuff->render_target);

  // Screen size and position stuff
  int w = Global->windowx, h = Global->windowy,
      rtw = Global->SRstuff->render_target->w,
      rth = Global->SRstuff->render_target->h;
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
  SDL_Texture* temptexture = SDL_CreateTextureFromSurface(
      Global->SRstuff->renderer, Global->SRstuff->render_target);
  SDL_SetTextureScaleMode(temptexture, SDL_SCALEMODE_PIXELART);
  SDL_RenderTexture(Global->SRstuff->renderer, temptexture, NULL, &temprect);
  SDL_RenderPresent(Global->SRstuff->renderer);
}

void openglrender() {
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT);
  glColor4f(1, 1, 1, 1);
  // OpenGL rendering goes here
  for (int i = 0; i < Global->mapfaces.size(); i++) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,
                  Global->GLstuff->textures[Global->mapfaces[i].texture]);
    glBegin(GL_TRIANGLES);
    if (Editor->currentlyselectedface == i)
      glColor4f(0, 1, 1, 1);
    else
      glColor4f(1, 1, 1, 1);
    for (int j = 0; j < 3; j++) {
      glm::vec2 pos = (glm::vec2)Global->Points[Global->mapfaces[i].points[j]] -
                      Editor->pos;
      pos *= Editor->zoom;
      pos *= -1;

      glm::vec2 uvw = Global->mapfaces[i].UVs[j];
      glTexCoord2f(uvw.x * Global->mapfaces[i].xloop,
                   uvw.y * Global->mapfaces[i].yloop);
      glVertex2f(pos.x * 2 / (float)Settings->resolutionx,
                 pos.y * 2 / (float)Settings->resolutiony);
    }
    glEnd();
  }

  glDisable(GL_TEXTURE_2D);

  for (int i = 0; i < Global->mapfaces.size(); i++) {
    glBegin(GL_LINE_LOOP);
    if (Editor->currentlyselectedface == i)
      glColor4f(0, 1, 1, 1);
    else
      glColor4f(1, 1, 1, 1);
    for (int j = 0; j < 3; j++) {
      glm::vec2 pos = (glm::vec2)Global->Points[Global->mapfaces[i].points[j]] -
                      Editor->pos;
      pos *= Editor->zoom;
      pos *= -1;

      glm::vec2 uvw = Global->mapfaces[i].UVs[j];
      glTexCoord2f(uvw.x * Global->mapfaces[i].xloop,
                   uvw.y * Global->mapfaces[i].yloop);
      glVertex2f(pos.x * 2 / (float)Settings->resolutionx,
                 pos.y * 2 / (float)Settings->resolutiony);
    }
    glEnd();
  }

  if (Editor->zoom >= 1) {
    int cnt = Settings->resolutionx / Editor->zoom;
    for (int i = -cnt - Editor->pos.x; i < cnt - Editor->pos.x; i++) {
      glBegin(GL_LINES);
      float pos = i + Editor->pos.x;
      pos *= Editor->zoom;
      glColor4f(1, 1, 1, 0.5f);
      glVertex2f(pos * 2 / (float)Settings->resolutionx, -1);
      glVertex2f(pos * 2 / (float)Settings->resolutionx, 1);
      glEnd();
    }

    cnt = Settings->resolutiony / Editor->zoom;
    for (int i = -cnt - Editor->pos.y; i < cnt - Editor->pos.y; i++) {
      glBegin(GL_LINES);
      float pos = i + Editor->pos.y;
      pos *= Editor->zoom;
      glColor4f(1, 1, 1, 0.5f);
      glVertex2f(-1, pos * 2 / (float)Settings->resolutiony);
      glVertex2f(1, pos * 2 / (float)Settings->resolutiony);
      glEnd();
    }
  }

  for (int i = 0; i < Global->Points.size(); i++) {
    glBegin(GL_TRIANGLE_FAN);

    glm::vec2 pos = (glm::vec2)Global->Points[i] - Editor->pos;
    pos *= Editor->zoom;
    pos *= -1;

    if (Editor->currentlyselectedpoint == i)
      glColor4f(0, 1, 1, 1);
    else
      glColor4f(1, 1, 1, 1);

    glVertex2f((pos.x - 2) * 2 / (float)Settings->resolutionx,
               (pos.y - 2) * 2 / (float)Settings->resolutiony);
    glVertex2f((pos.x - 2) * 2 / (float)Settings->resolutionx,
               (pos.y + 2) * 2 / (float)Settings->resolutiony);
    glVertex2f((pos.x + 2) * 2 / (float)Settings->resolutionx,
               (pos.y + 2) * 2 / (float)Settings->resolutiony);
    glVertex2f((pos.x + 2) * 2 / (float)Settings->resolutionx,
               (pos.y - 2) * 2 / (float)Settings->resolutiony);

    glEnd();
  }

  if (Global->pause || Global->isopeningfile) {
    glBegin(GL_TRIANGLE_FAN);

    glColor4f(0, 0, 0, 0.5f);

    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);

    glEnd();
  }

  renderUI();

  // glOrtho(0, Settings->resolutionx, Settings->resolutiony, 0, -1, 1);
  // glLoadIdentity();

  // glTranslatef(-Camera->position.x, -Camera->position.z,
  // -Camera->position.y);

  glFlush();

  SDL_GL_SwapWindow(Global->window);
}

void render() {
  switch (Settings->graphicsmode) {
    case 1:
      openglrender();
      break;
    default:
      softwarerender();
  }
}