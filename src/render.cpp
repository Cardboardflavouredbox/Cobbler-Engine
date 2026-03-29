#include <SDL3/SDL.h>

#include <cmath>
#include <deque>
#include <string>

#include "extern.h"
#include "map.h"
#include "screen.h"
#include "update.h"

float getdistancething(Vector3 P) {
  Vector3 p1;
  p1.x = P.x - Camera->position.x;
  p1.y = P.y - Camera->position.y;
  p1.z = P.z - Camera->position.z;
  float ps = std::sin(Camera->dir.x * 3.14 / 180.f);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.f);
  float what = std::sin(Camera->dir.y * 3.14 / 180.f);
  return p1.y * pc - p1.x * ps + p1.z * what;
}

ScreenPoint ToScreenSpace(Vector3 P) {
  Vector3 p1 = subVec3(P, Camera->position);

  float ps = std::sin(Camera->dir.x * 3.14 / 180.f);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.f);
  float whats = std::sin(Camera->dir.y * 3.14 / 180.f);
  float whatc = std::cos(Camera->dir.y * 3.14 / 180.f);

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

float Vector2Dot(Vector2 P1, Vector2 P2) {
  float deltaX = P2.x - P1.x;
  float deltaY = P2.y - P1.y;
  return std::sqrt(deltaX * deltaX + deltaY * deltaY);
}

float Areathing(Vector2 a, Vector2 b, Vector2 c) {
  return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}
Vector3 GetUV(Vector2 P, ScreenPoint R1, ScreenPoint R2, ScreenPoint R3) {
  float det = Areathing(R1.p, R2.p, R3.p);
  float a[3] = {Areathing(R2.p, R3.p, P), Areathing(R3.p, R1.p, P),
                Areathing(R1.p, R2.p, P)};
  for (int i = 0; i < 3; i++) a[i] /= det;
  return Vector3({a[0], a[1], a[2]});
}
void DrawLine(unsigned char* pixels, unsigned char pixelsdepth[], int pitch,
              unsigned char color, Vector3 rawvectors[]) {
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
          if (pixelsdepth[i + tempy * pitch] >= 8) {
            pixelsdepth[i + tempy * pitch] = 8;
            pixels[i + tempy * pitch] = color;
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
          if (pixelsdepth[tempx + i * pitch] >= 8) {
            pixelsdepth[tempx + i * pitch] = 8;
            pixels[tempx + i * pitch] = color;
          }
        }
      }
    }
  }
}

void DrawCircle(unsigned char* pixels, unsigned char pixelsdepth[], int pitch,
                unsigned char color, Vector3 rawpoint, int radius) {
  ScreenPoint point = ToScreenSpace(rawpoint);
  if (!point.isbehindcamera)
    for (int i = point.p.x - radius; i <= point.p.x + radius; i++) {
      for (int j = point.p.y - radius; j < point.p.y + radius; j++) {
        if (i > -1 && i < Settings->resolutionx && j > -1 &&
            j < Settings->resolutiony) {
          if (pixelsdepth[i + j * pitch] >= 7) {
            pixelsdepth[i + j * pitch] = 7;
            pixels[i + j * pitch] = color;
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

void DrawTri(unsigned char* pixels, unsigned char pixelsdepth[], int pitch,
             int texture, Vector3 rawvectors[], Vector2 UVs[], int xloop,
             int yloop) {
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
    for (int i = x; i <= x2; i++) {
      for (int j = y; j <= y2; j++) {
        Vector2 temp;
        temp.x = i;
        temp.y = j;
        if (temp.x >= 0 && temp.y >= 0 && temp.x < Settings->resolutionx &&
            temp.y < Settings->resolutiony) {
          if (Vector2inTri(temp, Vector2({vectors[0].p.x, vectors[0].p.y}),
                           Vector2({vectors[1].p.x, vectors[1].p.y}),
                           Vector2({vectors[2].p.x, vectors[2].p.y}))) {
            Vector3 uvw = GetUV(temp, vectors[0], vectors[1], vectors[2]);
            Vector2 uvresult = addVec2(
                addVec2(
                    multiplyVec2(multiplyVec2(UVs[0], uvw.x), vectors[0].dist),
                    multiplyVec2(multiplyVec2(UVs[1], uvw.y), vectors[1].dist)),
                multiplyVec2(multiplyVec2(UVs[2], uvw.z), vectors[2].dist));
            uvresult = multiplyVec2(uvresult, (1 / (uvw.x * vectors[0].dist +
                                                    uvw.y * vectors[1].dist +
                                                    uvw.z * vectors[2].dist)));

            int uvxthing = (int(128 * (uvresult.x)) * xloop) % 128;
            int uvything = (int(128 * (uvresult.y)) * yloop) % 128;
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
            if (pixelsdepth[i + j * pitch] > dist * 4) {
              r -= dist * 4;
              g -= dist * 4;
              b -= dist * 4;
              // r = uvresult.x * 255;
              // g = uvresult.y * 255;
              // b = 0;
              if (r < 0) r = 0;
              if (g < 0) g = 0;
              if (b < 0) b = 0;
              pixels[i + j * pitch] =
                  SDL_MapSurfaceRGB(Global->render_target, r, g, b);
              if (dist < 0) dist = 0;
              pixelsdepth[i + j * pitch] = (unsigned char)dist * 4;
            }
          }
        }
      }
    }
  }
}

float getinternaldivisionthing(Vector3 p1, Vector3 d, Vector3 p2) {
  float dist1 = getVec3dist(p1, d), dist2 = getVec3dist(d, p2);
  return dist1 / (dist1 + dist2);
}

Vector2 divisiontoVec2(Vector2 p1, Vector2 p2, float t) {
  return Vector2({p2.x * t + p1.x * (1 - t), p2.y * t + p1.y * (1 - t)});
}

Vector3 CutLinething(Vector3 invisible, Vector3 visible) {
  Vector3 p1, p2;
  p1.x = invisible.x - Camera->position.x;
  p1.y = invisible.y - Camera->position.y;
  p1.z = invisible.z - Camera->position.z;
  p2.x = visible.x - Camera->position.x;
  p2.y = visible.y - Camera->position.y;
  p2.z = visible.z - Camera->position.z;
  float ps = std::sin(Camera->dir.x * 3.14 / 180.f);
  float pc = std::cos(Camera->dir.x * 3.14 / 180.f);
  float what = std::sin(Camera->dir.y * 3.14 / 180.f);

  float u = (p1.y * pc - p1.x * ps + p1.z * what - 0.25f) /
            (-ps * (p1.x - p2.x) + pc * (p1.y - p2.y) + what * (p1.z - p2.z));

  Vector3 result =
      addVec3(invisible, multiplyVec3(subVec3(visible, invisible), u));
  return result;
}

void rendergame(unsigned char* pixels, unsigned char pixelsdepth[], int pitch) {
  std::deque<Mapface> tempmapfacedeque = Global->mapfaces, addlaterfacedeque;
  std::deque<Vector3> temppointsdeque = Global->Points;

  for (int i = 0; i < tempmapfacedeque.size(); i++) {
    Mapface* tempmapface = &tempmapfacedeque[i];
    float dist[3] = {};
    std::deque<int> invisibledeque, visibledeque;
    int invisiblecount = 0;
    for (int j = 0; j < 3; j++) {
      dist[j] = getdistancething(temppointsdeque[tempmapface->points[j]]);
      if (dist[j] < 0.25f) {
        invisiblecount++;
        invisibledeque.push_back(j);
      } else
        visibledeque.push_back(j);
    }
    switch (invisiblecount) {
      case 3: {
        tempmapfacedeque.erase(tempmapfacedeque.begin() + i);
        i--;
        break;
      }
      case 2: {
        for (int j = 0; j < 2; j++) {
          Vector3 newvec3 = CutLinething(
              temppointsdeque[tempmapface->points[invisibledeque[j]]],
              temppointsdeque[tempmapface->points[visibledeque[0]]]);
          temppointsdeque.push_back(newvec3);

          float internal = getinternaldivisionthing(
              temppointsdeque[tempmapface->points[invisibledeque[j]]], newvec3,
              temppointsdeque[tempmapface->points[visibledeque[0]]]);

          tempmapface->UVs[invisibledeque[j]] =
              divisiontoVec2(tempmapface->UVs[invisibledeque[j]],
                             tempmapface->UVs[visibledeque[0]], internal);

          tempmapface->points[invisibledeque[j]] = temppointsdeque.size() - 1;
        }
        break;
      }
      case 1: {
        Mapface newface;
        newface.texture = tempmapface->texture;
        newface.xloop = tempmapface->xloop;
        newface.yloop = tempmapface->yloop;
        newface.doublesided = tempmapface->doublesided;
        newface.points.resize(3);
        newface.UVs.resize(3);
        newface.points[visibledeque[0]] = tempmapface->points[visibledeque[0]];
        newface.UVs[visibledeque[0]] = tempmapface->UVs[visibledeque[0]];
        Vector3 newvec3;
        newvec3 = CutLinething(
            temppointsdeque[tempmapface->points[invisibledeque[0]]],
            temppointsdeque[tempmapface->points[visibledeque[0]]]);

        float internal = getinternaldivisionthing(
            temppointsdeque[tempmapface->points[invisibledeque[0]]], newvec3,
            temppointsdeque[tempmapface->points[visibledeque[0]]]);

        newface.UVs[invisibledeque[0]] =
            divisiontoVec2(tempmapface->UVs[invisibledeque[0]],
                           tempmapface->UVs[visibledeque[0]], internal);

        temppointsdeque.push_back(newvec3);

        newface.points[invisibledeque[0]] = temppointsdeque.size() - 1;
        newvec3 = CutLinething(
            temppointsdeque[tempmapface->points[invisibledeque[0]]],
            temppointsdeque[tempmapface->points[visibledeque[1]]]);

        internal = getinternaldivisionthing(
            temppointsdeque[tempmapface->points[invisibledeque[0]]], newvec3,
            temppointsdeque[tempmapface->points[visibledeque[1]]]);

        newface.UVs[visibledeque[1]] =
            divisiontoVec2(tempmapface->UVs[invisibledeque[0]],
                           tempmapface->UVs[visibledeque[1]], internal);

        temppointsdeque.push_back(newvec3);
        tempmapface->points[invisibledeque[0]] = temppointsdeque.size() - 1;
        tempmapface->UVs[invisibledeque[0]] = newface.UVs[visibledeque[1]];
        newface.points[visibledeque[1]] = temppointsdeque.size() - 1;
        addlaterfacedeque.push_back(newface);
        break;
      }
    }
  }

  tempmapfacedeque.insert(tempmapfacedeque.end(), addlaterfacedeque.begin(),
                          addlaterfacedeque.end());

  if (Global->rendermode == 0) {
    for (int k = 0; k < tempmapfacedeque.size(); k++) {
      Vector3 temp[3] = {temppointsdeque[tempmapfacedeque[k].points[0]],
                         temppointsdeque[tempmapfacedeque[k].points[1]],
                         temppointsdeque[tempmapfacedeque[k].points[2]]};
      Vector2 temp2[3] = {tempmapfacedeque[k].UVs[0],
                          tempmapfacedeque[k].UVs[1],
                          tempmapfacedeque[k].UVs[2]};
      DrawTri(pixels, pixelsdepth, pitch, tempmapfacedeque[k].texture, temp,
              temp2, tempmapfacedeque[k].xloop, tempmapfacedeque[k].yloop);
    }
  } else if (Global->rendermode == 1) {
    for (int k = 0; k < tempmapfacedeque.size(); k++) {
      Vector3 temp[2] = {temppointsdeque[tempmapfacedeque[k].points[0]],
                         temppointsdeque[tempmapfacedeque[k].points[1]]};
      DrawLine(pixels, pixelsdepth, pitch, 10, temp);
      temp[1] = temppointsdeque[tempmapfacedeque[k].points[2]];
      DrawLine(pixels, pixelsdepth, pitch, 10, temp);
      temp[0] = temppointsdeque[tempmapfacedeque[k].points[1]];
      DrawLine(pixels, pixelsdepth, pitch, 10, temp);
    }
    for (int k = 0; k < Global->Points.size(); k++) {
      DrawCircle(pixels, pixelsdepth, pitch,
                 (Global->editorselectedPoint == k ? 32 : 37),
                 Global->Points[k], 1);
      if (Global->editorselectedPoint == k) {
        Vector3 temp[2] = {Global->Points[k],
                           addVec3(Global->Points[k], Vector3({0, 4, 0}))};
        DrawLine(pixels, pixelsdepth, pitch, 40, temp);
        DrawCircle(pixels, pixelsdepth, pitch, 40, temp[1], 1);
        temp[1] = addVec3(Global->Points[k], Vector3({4, 0, 0}));
        DrawLine(pixels, pixelsdepth, pitch, 20, temp);
        DrawCircle(pixels, pixelsdepth, pitch, 20, temp[1], 1);
        temp[1] = addVec3(Global->Points[k], Vector3({0, 0, 4}));
        DrawLine(pixels, pixelsdepth, pitch, 50, temp);
        DrawCircle(pixels, pixelsdepth, pitch, 50, temp[1], 1);
      }
    }
  }
}

void drawUIsquare(unsigned char* pixels, unsigned char pixelsdepth[], int pitch,
                  Vector2 points[], unsigned char color) {
  for (int i = points[0].x; i < points[1].x; i++) {
    for (int j = points[0].y; j < points[1].y; j++) {
      pixelsdepth[i + j * pitch] = 0;
      pixels[i + j * pitch] = color;
    }
  }
}

void renderStringUI(unsigned char* pixels, unsigned char pixelsdepth[],
                    int pitch, std::string string, Vector2 coord,
                    unsigned char color) {
  int x = coord.x, y = coord.y;
  for (int i = 0; i < string.length(); i++) {
    FT_UInt temp = FT_Get_Char_Index(Global->FTface, string[i]);
    if (!Global->Glyphmap.contains(temp)) {
      FT_Load_Glyph(Global->FTface, temp, FT_LOAD_MONOCHROME);
      FT_Render_Glyph(Global->FTface->glyph, FT_RENDER_MODE_MONO);
      Global->Glyphmap[temp] = CreateGlyph(Global->FTface->glyph);
    }
    CustomGlyphthing glyph = Global->Glyphmap[temp];
    x += glyph.offsetx;
    int x2 = glyph.width, y2 = glyph.height;
    for (int j = 0; j < y2; j++) {
      for (int k = 0; k < x2; k++) {
        if (k > -1 && k < Settings->resolutionx && j > -1 &&
            j < Settings->resolutiony &&
            ((glyph.pixels[k / 8 + j * glyph.pitch]) >> (7 - k % 8) & 0x01)) {
          pixelsdepth[(k + x) + (j + y + 12 - glyph.offsety) * pitch] = 0;
          pixels[(k + x) + (j + y + 12 - glyph.offsety) * pitch] = color;
        }
      }
    }
    x += glyph.advancex;
    y += glyph.advancey;
  }
}

void renderUI(unsigned char* pixels, unsigned char pixelsdepth[], int pitch) {}

void render() {
  // SDL_SetRenderDrawColorFloat(Global->renderer, 0, 0, 0, 1);
  // SDL_RenderClear(Global->renderer);
  SDL_LockSurface(Global->render_target);

  unsigned char* pixels =
      static_cast<unsigned char*>(Global->render_target->pixels);
  int pitch = Global->render_target->pitch;
  unsigned char pixelsdepth[Settings->resolutionx * Settings->resolutiony];

  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      pixelsdepth[i + j * pitch] = 255;
    }
  }
  renderUI(pixels, pixelsdepth, pitch);
  rendergame(pixels, pixelsdepth, pitch);

  if (Global->rendermode == 0) {
    for (int i = 0; i < Settings->resolutionx; i++) {
      for (int j = 0; j < Settings->resolutiony; j++) {
        if (pixelsdepth[i + j * pitch] == 255) {
          Uint32 color = static_cast<Uint32*>(
              Global->textures[Global->skybox]->pixels)
              [(i + int((1 - ((int(Camera->dir.x) % 180) / 180.f)) * 640.f)) %
                   640 +
               (int((1 - (Camera->dir.y < 0 ? 0 : Camera->dir.y) / 90.f) *
                    200.f) +
                j) *
                   640];
          int r = (color >> 0) & 0xFF;
          int g = (color >> 8) & 0xFF;
          int b = (color >> 16) & 0xFF;
          int a = (color >> 24) & 0xFF;
          pixels[i + j * pitch] =
              SDL_MapSurfaceRGB(Global->render_target, r, g, b);
        }
      }
    }
  } else if (Global->rendermode == 1) {
    for (int i = 0; i < Settings->resolutionx; i++) {
      for (int j = 0; j < Settings->resolutiony; j++) {
        if (pixelsdepth[i + j * pitch] == 255) {
          pixels[i + j * pitch] = 1;
        }
      }
    }
  }

  if (Global->pause || Global->isopeningfile) {
    for (int i = 0; i < Settings->resolutionx; i++) {
      for (int j = 0; j < Settings->resolutiony; j++) {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i + j * pitch],
                   SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_INDEX8),
                   Global->palette, &r, &g, &b);
        int r2 = r, g2 = g, b2 = b;
        r2 -= 64;
        g2 -= 64;
        b2 -= 64;
        if (r2 < 0) r2 = 0;
        if (g2 < 0) g2 = 0;
        if (b2 < 0) b2 = 0;
        pixels[i + j * pitch] =
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