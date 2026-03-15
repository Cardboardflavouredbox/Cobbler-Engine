#include "render.h"

#include <SDL3/SDL.h>

#include <cmath>
#include <string>

#include "extern.h"
#include "map.h"

struct ScreenPoint {
  Vector3 p, uvw;
  bool isbehindcamera = false;
};

ScreenPoint drawPoint(Vector3 P) {
  Vector3 p1;
  p1.x = P.x - Camera->position.x;
  p1.y = P.y - Camera->position.y;
  p1.z = P.z - Camera->position.z;

  float ps = std::sin(Camera->dir.x * 3.14 / 180.f);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.f);
  float what = std::sin(Camera->dir.y * 3.14 / 180.f);

  float tx = p1.x * pc + p1.y * ps;
  float ty = p1.y * pc - p1.x * ps + p1.z * what;
  float tz = 180 * what;

  ScreenPoint screenpos;
  if (ty <= 0.5f) {
    screenpos.isbehindcamera = true;
    ty = 0.5f;
  }
  screenpos.p.x = (tx * Settings->fov / ty) + (Settings->resolutionx / 2);
  screenpos.p.y =
      (-p1.z * Settings->fov / ty) + (Settings->resolutiony / 2) + tz;
  // screenpos.p.z = ty;
  return screenpos;
}

float Vector2Dot(Vector2 P1, Vector2 P2) {
  float deltaX = P2.x - P1.x;
  float deltaY = P2.y - P1.y;
  return std::sqrt(deltaX * deltaX + deltaY * deltaY);
}

Vector3 GetUV(Vector2 P, ScreenPoint R1, ScreenPoint R2, ScreenPoint R3) {
    Vector3 UV;
  float det = (R2.y - R3.y) * (R1.x - R3.x) + (R3.x - R2.x) * (R1.y - R3.y);
  float factor_alpha =
      (R2.y - R3.y) * (P.x - R3.x) + (R3.x - R2.x) * (P.y - R3.y);
  float factor_beta =
      (R3.y - R1.y) * (P.x - R3.x) + (R1.x - R3.x) * (P.y - R3.y);
  UV.x = factor_alpha / det;
  UV.y = factor_beta / det;
  UV.z = 1.0 - UV.x - UV.y;
  uvw[0] /= R1.z;
  uvw[1] /= R1.z;
  uvw[2] /= R1.z;
  uvw[0] /= R2.z;
  uvw[1] /= R2.z;
  uvw[2] /= R2.z;
  uvw[0] /= R3.z;
  uvw[1] /= R3.z;
  uvw[2] /= R3.z;
  // Pre-compute 1 over z
  v0[2] = 1 / v0[2], v1[2] = 1 / v1[2], v2[2] = 1 / v2[2];
  return UV;
}
void DrawLine(unsigned char* pixels, int pitch, unsigned char color,
              Vector3 rawvectors[]) {
  ScreenPoint vectors[2] = {drawPoint(rawvectors[0]), drawPoint(rawvectors[1])};
  if (!vectors[0].isbehindcamera || !vectors[1].isbehindcamera) {
    for (int i = vectors[0].p.x; i < vectors[1].p.x; i++) {
      int y = vectors[0].p.y +
              ((i - vectors[0].p.x) * (vectors[1].p.y - vectors[0].p.y) /
               (vectors[1].p.x - vectors[0].p.x));
      if (i >= 0 && y >= 0 && i <= Settings->resolutionx &&
          y <= Settings->resolutiony) {
        pixels[i + y * pitch] = color;
      }
    }
    if (vectors[0].p.x == vectors[1].p.x) {
      for (int i = vectors[0].p.y; i < vectors[1].p.y; i++) {
        if (vectors[0].p.x >= 0 && i >= 0 &&
            vectors[0].p.x <= Settings->resolutionx &&
            i <= Settings->resolutiony) {
          pixels[(int)vectors[0].p.x + i * pitch] = color;
        }
      }
    }
  }
}

float anglething(Vector2 a, Vector2 b, Vector2 c) {
  Vector2 ab = Vector2({b.x - a.x, b.y - a.y});
  Vector2 cb = Vector2({b.x - c.x, b.y - c.y});

  float dot = (ab.x * cb.x + ab.y * cb.y);
  float cross = (ab.x * cb.y - ab.y * cb.x);

  float alpha = atan2f(cross, dot);

  return alpha * 180.f / 3.14f + 0.5f;
}
float Vector2inTri(Vector2 p, Vector2 v1, Vector2 v2, Vector2 v3) {
  float s1 = anglething(v3, p, v1), s2 = anglething(v1, p, v2),
        s3 = anglething(v2, p, v3);

  return (s1 + s2 + s3 > 360);
}

void DrawTri(unsigned char* pixels, int pitch, int texture,
             Vector3 rawvectors[], int xloop, int yloop) {
  ScreenPoint vectors[3] = {drawPoint(rawvectors[0]), drawPoint(rawvectors[1]),
                            drawPoint(rawvectors[2])};
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
    if (x >= Settings->resolutionx) x = Settings->resolutionx;
    if (x2 < 0) x2 = 0;
    if (x2 >= Settings->resolutionx) x2 = Settings->resolutionx;
    if (y < 0) y = 0;
    if (y >= Settings->resolutiony) y = Settings->resolutiony;
    if (y2 < 0) y2 = 0;
    if (y2 >= Settings->resolutiony) y2 = Settings->resolutiony;
    for (int i = x; i <= x2; i++) {
      for (int j = y; j <= y2; j++) {
        Vector2 temp;
        temp.x = i;
        temp.y = j;
        if (temp.x >= 0 && temp.y >= 0 && temp.x <= Settings->resolutionx &&
            temp.y <= Settings->resolutiony) {
          if (Vector2inTri(temp, Vector2({vectors[0].p.x, vectors[0].p.y}),
                           Vector2({vectors[1].p.x, vectors[1].p.y}),
                           Vector2({vectors[2].p.x, vectors[2].p.y}))) {
            Vector3 uvw = GetUV(temp, vectors[0].p, vectors[1].p, vectors[2].p);
            int uvxthing = (int(128 * (uvw.z + uvw.y)) * xloop) % 128;
            int uvything = (int(128 * (uvw.z)) * yloop) % 128;
            Uint32 color = static_cast<Uint32*>(
                Global->textures[texture]->pixels)[uvxthing + uvything * 128];

            int r = (color >> 0) & 0xFF;
            int g = (color >> 8) & 0xFF;
            int b = (color >> 16) & 0xFF;
            int a = (color >> 24) & 0xFF;

            Vector3 tempvec3;
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
            r -= dist * 4;
            g -= dist * 4;
            b -= dist * 4;
            if (r < 0) r = 0;
            if (g < 0) g = 0;
            if (b < 0) b = 0;
            pixels[i + j * pitch] =
                SDL_MapSurfaceRGB(Global->render_target, r, g, b);
          }
        }
      }
    }
  }
}

void render() {
  // SDL_SetRenderDrawColorFloat(Global->renderer, 0, 0, 0, 1);
  // SDL_RenderClear(Global->renderer);
  SDL_LockSurface(Global->render_target);

  unsigned char* pixels =
      static_cast<unsigned char*>(Global->render_target->pixels);
  int pitch = Global->render_target->pitch;
  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      pixels[i + j * pitch] = 0;
    }
  }

  for (int k = 0; k < Global->mapfaces.size(); k++) {
    if (Global->mapfaces[k].points.size() == 4) {
      Vector3 temp[3] = {Global->Points[Global->mapfaces[k].points[0]],
                         Global->Points[Global->mapfaces[k].points[1]],
                         Global->Points[Global->mapfaces[k].points[2]]};
      Vector3 temp2[3] = {Global->Points[Global->mapfaces[k].points[2]],
                          Global->Points[Global->mapfaces[k].points[3]],
                          Global->Points[Global->mapfaces[k].points[0]]};
      DrawTri(pixels, pitch, Global->mapfaces[k].texture, temp,
              Global->mapfaces[k].xloop, Global->mapfaces[k].yloop);
      DrawTri(pixels, pitch, Global->mapfaces[k].texture, temp2,
              Global->mapfaces[k].xloop, Global->mapfaces[k].yloop);
    } else {
      Vector3 temp[3] = {Global->Points[Global->mapfaces[k].points[0]],
                         Global->Points[Global->mapfaces[k].points[1]],
                         Global->Points[Global->mapfaces[k].points[2]]};
      DrawTri(pixels, pitch, Global->mapfaces[k].texture, temp,
              Global->mapfaces[k].xloop, Global->mapfaces[k].yloop);
    }
  }

  SDL_UnlockSurface(Global->render_target);

  int w, h, rtw = Global->render_target->w, rth = Global->render_target->h;
  SDL_GetWindowSizeInPixels(Global->window, &w, &h);

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