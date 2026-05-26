#include "render.h"

#include <cmath>

#include "extern.h"
#include "rendermath.h"
#include "screen.h"

ScreenPoint ToScreenSpace(glm::vec3 P) {
  glm::vec3 p1 = (P - Camera->position);

  float ps = std::sin(Camera->dir.x * PI / 180.f);
  float pc = std::cos(Camera->dir.x * PI / 180.f);
  float whats = std::sin(Camera->dir.y * PI / 180.f);

  float tx = p1.x * pc + p1.y * ps;
  float ty = p1.y * pc - p1.x * ps + p1.z * whats;
  float tz = 180 * whats;

  ScreenPoint screenpos;
  if (ty <= 0.25f) {
    screenpos.isbehindcamera = true;
    ty = 0.25f;
  }
  screenpos.p.x = (tx * Settings->fov / ty) + (Settings->resolutionx / 2);
  screenpos.p.y =
      (-p1.z * Settings->fov / ty) + (Settings->resolutiony / 2) + tz;
  screenpos.dist = 1.f / ty;
  return screenpos;
}

void DrawLine(unsigned char color, glm::vec3 rawvectors[]) {
  ScreenPoint vectors[2] = {ToScreenSpace(rawvectors[0]),
                            ToScreenSpace(rawvectors[1])};
  if (!vectors[0].isbehindcamera || !vectors[1].isbehindcamera) {
    int x = vectors[0].p.x, x2 = vectors[1].p.x;

    int y = vectors[0].p.y, y2 = vectors[1].p.y;

    if (std::abs(y2 - y) < std::abs(x2 - x)) {
      if (x > x2) {
        x = vectors[1].p.x;
        x2 = vectors[0].p.x;
        y = vectors[1].p.y;
        y2 = vectors[0].p.y;
      }
      for (int i = x; i <= x2; i++) {
        int tempy = y + ((i - x) * (y2 - y) / (x2 - x));
        if (i >= 0 && tempy >= 0 && i < Settings->resolutionx &&
            tempy < Settings->resolutiony) {
          if (Global->SRstuff
                  ->pixelsdepth[i + tempy * Global->SRstuff->pitch] >= 8) {
            Global->SRstuff->pixelsdepth[i + tempy * Global->SRstuff->pitch] =
                8;
            Global->SRstuff->pixels[i + tempy * Global->SRstuff->pitch] = color;
          }
        }
      }
    } else {
      if (y > y2) {
        x = vectors[1].p.x;
        x2 = vectors[0].p.x;
        y = vectors[1].p.y;
        y2 = vectors[0].p.y;
      }
      for (int i = y; i <= y2; i++) {
        int tempx = x + ((i - y) * (x2 - x) / (y2 - y));
        if (tempx >= 0 && i >= 0 && tempx < Settings->resolutionx &&
            i < Settings->resolutiony) {
          if (Global->SRstuff
                  ->pixelsdepth[tempx + i * Global->SRstuff->pitch] >= 8) {
            Global->SRstuff->pixelsdepth[tempx + i * Global->SRstuff->pitch] =
                8;
            Global->SRstuff->pixels[tempx + i * Global->SRstuff->pitch] = color;
          }
        }
      }
    }
  }
}

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

void DrawTri(std::string texture, glm::vec3 rawvectors[], glm::vec2 UVs[],
             int xloop, int yloop) {
  ScreenPoint vectors[3] = {ToScreenSpace(rawvectors[0]),
                            ToScreenSpace(rawvectors[1]),
                            ToScreenSpace(rawvectors[2])};
  if (!vectors[0].isbehindcamera || !vectors[1].isbehindcamera ||
      !vectors[2].isbehindcamera) {
    int x = vectors[0].p.x, x2 = vectors[0].p.x, y = vectors[0].p.y,
        y2 = vectors[0].p.y;
    for (int i = 1; i < 3; i++) {
      if (vectors[i].p.x < x) x = vectors[i].p.x;
      if (vectors[i].p.x > x2) x2 = vectors[i].p.x;
      if (vectors[i].p.y < y) y = vectors[i].p.y;
      if (vectors[i].p.y > y2) y2 = vectors[i].p.y;
    }
    if (x < 0) x = 0;
    if (x >= Settings->resolutionx - 1) x = Settings->resolutionx - 1;
    if (x2 < 0) x2 = 0;
    if (x2 >= Settings->resolutionx - 1) x2 = Settings->resolutionx - 1;
    if (y < 0) y = 0;
    if (y >= Settings->resolutiony - 1) y = Settings->resolutiony - 1;
    if (y2 < 0) y2 = 0;
    if (y2 >= Settings->resolutiony - 1) y2 = Settings->resolutiony - 1;

    float det = Areathing(vectors[0].p, vectors[1].p, vectors[2].p);
    glm::vec3 A = {(vectors[2].p.x - vectors[1].p.x),
                   (vectors[0].p.x - vectors[2].p.x),
                   (vectors[1].p.x - vectors[0].p.x)},
              B = {(vectors[1].p.y - vectors[2].p.y),
                   (vectors[2].p.y - vectors[0].p.y),
                   (vectors[0].p.y - vectors[1].p.y)};
    A /= -det;
    B /= -det;
    glm::vec3 uvwrow =
        GetUV(glm::vec2(x, y), vectors[0].p, vectors[1].p, vectors[2].p);
    for (int i = x; i <= x2; i++) {
      glm::vec3 uvw = uvwrow;
      for (int j = y; j <= y2; j++) {
        glm::vec2 temp;
        temp.x = i;
        temp.y = j;
        if (temp.x >= 0 && temp.y >= 0 && temp.x < Settings->resolutionx &&
            temp.y < Settings->resolutiony) {
          if (Vec2inTri(temp, vectors[0].p, vectors[1].p, vectors[2].p)) {
            glm::vec2 uvresult = ((((UVs[0] * uvw.x) * vectors[0].dist) +
                                   ((UVs[1] * uvw.y) * vectors[1].dist)) +
                                  ((UVs[2] * uvw.z) * vectors[2].dist));
            uvresult = (uvresult * (1 / (uvw.x * vectors[0].dist +
                                         uvw.y * vectors[1].dist +
                                         uvw.z * vectors[2].dist)));

            int uvxthing = (int(128 * (uvresult.x)) * xloop) % 128;
            int uvything = (int(128 * (uvresult.y)) * yloop) % 128;
            Uint8 color =
                static_cast<Uint8*>(Global->SRstuff->textures[texture]
                                        ->pixels)[uvxthing + uvything * 128];

            glm::vec3 tempvec3;
            tempvec3.x = rawvectors[0].x * uvw.x + rawvectors[1].x * uvw.y +
                         rawvectors[2].x * uvw.z;
            tempvec3.y = rawvectors[0].y * uvw.x + rawvectors[1].y * uvw.y +
                         rawvectors[2].y * uvw.z;
            tempvec3.z = rawvectors[0].z * uvw.x + rawvectors[1].z * uvw.y +
                         rawvectors[2].z * uvw.z;
            tempvec3.x -= Camera->position.x;
            tempvec3.y -= Camera->position.y;
            tempvec3.z -= Camera->position.z;
            float dist =
                std::sqrt(tempvec3.x * tempvec3.x + tempvec3.y * tempvec3.y +
                          tempvec3.z * tempvec3.z);
            if (Global->SRstuff->pixelsdepth[i + j * Global->SRstuff->pitch] >
                dist * 3) {
              Global->SRstuff->pixels[i + j * Global->SRstuff->pitch] = color;
              if (dist < 0) dist = 0;
              Global->SRstuff->pixelsdepth[i + j * Global->SRstuff->pitch] =
                  (unsigned char)dist * 4;
            }
          }
          uvw += A;
        }
      }
      uvwrow += B;
    }
  }
}