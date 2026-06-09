#include "render.h"

#include <SDL3/SDL_log.h>

#include <cmath>
#include <glm/gtc/quaternion.hpp>

#include "extern.h"
#include "rendermath.h"
#include "screen.h"

ScreenPoint ToScreenSpace(glm::vec3 P) {
  glm::vec3 p1 = (P - Camera->position);

  float ps = std::sin(Camera->dir.x * PI / 180.f);
  float pc = std::cos(Camera->dir.x * PI / 180.f);

  glm::quat q = glm::angleAxis(float(Camera->dir.y * PI / 180.f),
                               glm::vec3(-pc, -ps, 0.0f));

  p1 = q * p1;

  float tx = p1.x * pc + p1.y * ps;
  float ty = p1.y * pc - p1.x * ps;

  ScreenPoint screenpos;
  if (ty <= 0.25f) {
    screenpos.isbehindcamera = true;
    ty = 0.25f;
  }
  screenpos.p.x = (tx * Settings->fov / ty) + (Settings->resolutionx / 2);
  screenpos.p.y = (-p1.z * Settings->fov / ty) + (Settings->resolutiony / 2);
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
            int texturew = Global->SRstuff->textures[texture]->w,
                textureh = Global->SRstuff->textures[texture]->h;
            int uvxthing = (int(texturew * (uvresult.x)) * xloop) % texturew;
            int uvything = (int(textureh * (uvresult.y)) * yloop) % textureh;
            Uint8 color = static_cast<Uint8*>(
                Global->SRstuff->textures[texture]
                    ->pixels)[uvxthing + uvything * texturew];

            if (color > 0) {
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
          }
          uvw += A;
        }
      }
      uvwrow += B;
    }
  }
}

glm::vec3 modelapplybones(GlobalClass::Model::Vertex input,
                          ModelGroupClass* modelgroup, unsigned int frame) {
  glm::vec3 temp = input.pos;

  std::string tempstr = input.bone;
  while (tempstr != "null") {
    ModelGroupClass::Bone* bone = &modelgroup->Bonemap[tempstr];

    glm::vec3 pos = bone->Poses.begin()->second.pos,
              scale = bone->Poses.begin()->second.scale;
    glm::quat rot = bone->Poses.begin()->second.rot;
    unsigned int framebefore = modelgroup->animstart;

    // SDL_Log("%d %d %s", modelgroup->Bonemap.size(), bone->Poses.size(),
    //         tempstr.c_str());
    for (auto const& [key, val] : bone->Poses) {
      SDL_Log("key: %u", key);
      if (frame == key) {
        pos = val.pos;
        rot = val.rot;
        scale = val.scale;
        break;
      } else if (frame > key) {
        pos = val.pos;
        rot = val.rot;
        scale = val.scale;
      } else {
        float a = (frame - framebefore) / float(key - framebefore);
        rot = glm::slerp(rot, val.rot, a);
        pos = pos * (1 - a) + val.pos * a;
        scale = scale * (1 - a) + val.scale * a;
        break;
      }
    }

    glm::quat final_quat = rot;

    temp = (final_quat) * (temp - bone->head);
    temp += bone->head;
    temp += pos;
    temp.x *= scale.x;
    temp.y *= scale.y;
    temp.z *= scale.z;
    tempstr = bone->parent;
  }
  return temp;
}

void renderModelGroup(Modeltransform modeltrans, ModelGroupClass* modelgroup) {
  glm::vec3 renderpos = modeltrans.position - Camera->position;
  switch (Settings->graphicsmode) {
    case 1: {
      glDisable(GL_CULL_FACE);
      for (int a = 0; a < modelgroup->Models.size(); a++) {
        GlobalClass::Model* model = &Global->Modelmap[modelgroup->Models[a]];
        for (int j = 0; j < model->faces.size(); j++) {
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D,
                        Global->GLstuff->textures[model->texture]);
          glBegin(GL_TRIANGLES);
          for (int k = 2; k >= 0; k--) {
            glm::vec3 pos =
                modelapplybones(model->points[model->faces[j].point[k]],
                                modelgroup, (unsigned int)(modeltrans.frame));
            pos.x *= modeltrans.size.x;
            pos.y *= modeltrans.size.y;
            pos.z *= modeltrans.size.z;
            pos += renderpos;
            glTexCoord2f(model->faces[j].uv[k].x, 1 - model->faces[j].uv[k].y);
            glVertex3f(pos.x, pos.y, pos.z);
          }
          glEnd();
        }
      }
      glEnable(GL_CULL_FACE);
      break;
    }
      // default: {
      //   for (int j = 0; j < model->faces.size(); j++) {
      //     glm::vec3 vec[3];
      //     glm::vec2 uv[3];
      //     for (int k = 0; k < 3; k++) {
      //       glm::vec3 pos = model->points[model->faces[j].point[k]];
      //       pos.x *= Global->Models[i].size.x;
      //       pos.y *= Global->Models[i].size.y;
      //       pos.z *= Global->Models[i].size.z;
      //       pos += renderpos;
      //       vec[k] = pos;
      //       uv[k] = glm::vec2(
      //           {model->faces[j].uv[k].x, 1 - model->faces[j].uv[k].y});
      //     }
      //     DrawTri(model->texture, vec, uv, 1, 1);
      //   }
      // }
  }
}