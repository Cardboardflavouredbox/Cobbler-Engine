#include "render.h"

#include <SDL3/SDL_log.h>

#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <utility>

#include "camera.h"
#include "extern.h"
#include "pi.h"
#include "rendermath.h"
#include "screen.h"
#include "settings.h"

// function that turns vec3 into 2d point on screen.
// for software rendering.
ScreenPoint ToScreenSpace(glm::vec3 P) {
  glm::vec3 p1 = (P - LocalPlayer->position);

  float ps = std::sin(LocalPlayer->dir.x * PI / 180.f);
  float pc = std::cos(LocalPlayer->dir.x * PI / 180.f);

  glm::quat q = glm::angleAxis(float(LocalPlayer->dir.y * PI / 180.f),
                               glm::vec3(-pc, -ps, 0.0f));

  p1 = q * p1;

  float tx = p1.x * pc + p1.y * ps;
  float ty = p1.y * pc - p1.x * ps;

  ScreenPoint screenpos;
  if (ty <= 0.25f) {
    screenpos.isbehindLocalPlayer = true;
    ty = 0.25f;
  }
  screenpos.p.x = (tx * Settings->fov / ty) + (Settings->resolutionx / 2);
  screenpos.p.y = (-p1.z * Settings->fov / ty) + (Settings->resolutiony / 2);
  screenpos.dist = 1.f / ty;
  return screenpos;
}

// function for drawing line in software renderer.
void DrawLine(unsigned char color, glm::vec3 rawvectors[]) {
  ScreenPoint vectors[2] = {ToScreenSpace(rawvectors[0]),
                            ToScreenSpace(rawvectors[1])};
  if (!vectors[0].isbehindLocalPlayer || !vectors[1].isbehindLocalPlayer) {
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
          if (RendererGlobal->SRstuff
                  ->pixelsdepth[i + tempy * RendererGlobal->SRstuff->pitch] >=
              8) {
            RendererGlobal->SRstuff
                ->pixelsdepth[i + tempy * RendererGlobal->SRstuff->pitch] = 8;
            RendererGlobal->SRstuff
                ->pixels[i + tempy * RendererGlobal->SRstuff->pitch] = color;
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
          if (RendererGlobal->SRstuff
                  ->pixelsdepth[tempx + i * RendererGlobal->SRstuff->pitch] >=
              8) {
            RendererGlobal->SRstuff
                ->pixelsdepth[tempx + i * RendererGlobal->SRstuff->pitch] = 8;
            RendererGlobal->SRstuff
                ->pixels[tempx + i * RendererGlobal->SRstuff->pitch] = color;
          }
        }
      }
    }
  }
}

// this actually draws squares. will change later.
void DrawCircle(unsigned char color, glm::vec3 rawpoint, int radius) {
  ScreenPoint point = ToScreenSpace(rawpoint);
  if (!point.isbehindLocalPlayer)
    for (int i = point.p.x - radius; i <= point.p.x + radius; i++) {
      for (int j = point.p.y - radius; j < point.p.y + radius; j++) {
        if (i > -1 && i < Settings->resolutionx && j > -1 &&
            j < Settings->resolutiony) {
          if (RendererGlobal->SRstuff
                  ->pixelsdepth[i + j * RendererGlobal->SRstuff->pitch] >= 7) {
            RendererGlobal->SRstuff
                ->pixelsdepth[i + j * RendererGlobal->SRstuff->pitch] = 7;
            RendererGlobal->SRstuff
                ->pixels[i + j * RendererGlobal->SRstuff->pitch] = color;
          }
        }
      }
    }
}

// triangle drawing function for software renderer.
void DrawTri(std::string texture, glm::vec3 rawvectors[], glm::vec2 UVs[]) {
  ScreenPoint vectors[3] = {ToScreenSpace(rawvectors[0]),
                            ToScreenSpace(rawvectors[1]),
                            ToScreenSpace(rawvectors[2])};
  if (!vectors[0].isbehindLocalPlayer || !vectors[1].isbehindLocalPlayer ||
      !vectors[2].isbehindLocalPlayer) {
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
            int texturew = RendererGlobal->SRstuff->textures[texture]->w,
                textureh = RendererGlobal->SRstuff->textures[texture]->h;
            int uvxthing = (int(texturew * (uvresult.x))) % texturew;
            int uvything = (int(textureh * (uvresult.y))) % textureh;
            uint8_t color = static_cast<uint8_t*>(
                RendererGlobal->SRstuff->textures[texture]
                    ->pixels)[uvxthing + uvything * texturew];

            if (color > 0) {
              glm::vec3 tempvec3;
              tempvec3.x = rawvectors[0].x * uvw.x + rawvectors[1].x * uvw.y +
                           rawvectors[2].x * uvw.z;
              tempvec3.y = rawvectors[0].y * uvw.x + rawvectors[1].y * uvw.y +
                           rawvectors[2].y * uvw.z;
              tempvec3.z = rawvectors[0].z * uvw.x + rawvectors[1].z * uvw.y +
                           rawvectors[2].z * uvw.z;
              tempvec3.x -= LocalPlayer->position.x;
              tempvec3.y -= LocalPlayer->position.y;
              tempvec3.z -= LocalPlayer->position.z;
              float dist =
                  std::sqrt(tempvec3.x * tempvec3.x + tempvec3.y * tempvec3.y +
                            tempvec3.z * tempvec3.z);
              if (RendererGlobal->SRstuff
                      ->pixelsdepth[i + j * RendererGlobal->SRstuff->pitch] >
                  dist * 3) {
                RendererGlobal->SRstuff
                    ->pixels[i + j * RendererGlobal->SRstuff->pitch] = color;
                if (dist < 0) dist = 0;
                RendererGlobal->SRstuff
                    ->pixelsdepth[i + j * RendererGlobal->SRstuff->pitch] =
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

// apply animations of bones.
void modelapplybones(Modeltransform* modeltrans, uint32_t actioncode,
                     ModelGroupClass* modelgroup, float frame, float lookdir) {
  for (auto& [code, bone] : modelgroup->Bonemap) {
    Modeltransform::BoneResult* boneresult = &modeltrans->Bonemap[code];
    boneresult->head = bone.head;
    boneresult->rot = glm::quat(1, 0, 0, 0);
    boneresult->scale = glm::vec3(1);
  }

  for (auto& [code, bone] : modelgroup->Bonemap) {
    uint32_t boneindex = code;
    Modeltransform::BoneResult* boneresult = &modeltrans->Bonemap[code];
    // apply bones
    while (boneindex != uint32_t(-1)) {
      ModelGroupClass::Bone* bone = &modelgroup->Bonemap[boneindex];

      glm::vec3 pos = glm::vec3(0), scale = glm::vec3(1);
      glm::quat rot = glm::quat(1, 0, 0, 0);
      if (bone->Poses.empty()) {
        // SDL_Log("no poses lol");
      } else {
        pos = bone->Poses[actioncode].begin()->second.pos;
        scale = bone->Poses[actioncode].begin()->second.scale;
        rot = bone->Poses[actioncode].begin()->second.rot;
        uint32_t framebefore = modelgroup->anim[actioncode][0];

        for (auto const& [key, val] : bone->Poses[actioncode]) {
          if (frame == key) {
            pos = val.pos;
            rot = val.rot;
            scale = val.scale;
            break;
          } else if (frame > key) {
            pos = val.pos;
            rot = val.rot;
            scale = val.scale;
            framebefore = key;
          } else {
            float a = ((float)frame - (float)framebefore) /
                      ((float)key - (float)framebefore);
            // lerp values.
            rot = glm::mix(rot, val.rot, a);
            pos = glm::mix(pos, val.pos, a);
            scale = glm::mix(scale, val.scale, a);
            break;
          }
        }
      }

      // the axis translation code of fear and despair...
      float angle = glm::angle(rot);

      glm::vec3 boneaxis = glm::normalize(bone->tail - bone->head);

      glm::vec3 axis =
          glm::quatLookAt(glm::vec3(0, 1, 0), boneaxis) * glm::axis(rot);

      axis = (glm::length(axis) > 0.0001f) ? glm::normalize(axis)
                                           : glm::vec3(0, 1, 0);

      glm::quat final_quat = glm::angleAxis(angle, axis);

      // if (pos != glm::vec3(0) || scale != glm::vec3(1) ||
      //     final_quat != glm::quat(1, 0, 0, 0))
      //   check = true;

      // some lil correction for some bones.
      // find a way to not hardcode this!
      if (boneindex == BonetoInt["Spine"]) {
        float tempdir = lookdir;

        if (tempdir > 0)
          tempdir /= 3.f;
        else
          tempdir /= 2.f;

        final_quat = final_quat *
                     glm::angleAxis(glm::radians(tempdir), glm::vec3(1, 0, 0));
      } else if (boneindex == BonetoInt["Head"]) {
        float tempdir = lookdir;

        if (tempdir > 0)
          tempdir *= 2.f / 3.f;
        else
          tempdir /= 2.f;

        final_quat = final_quat *
                     glm::angleAxis(glm::radians(tempdir), glm::vec3(1, 0, 0));
      } else if (boneindex == BonetoInt["Arm.L"]) {
        float tempdir = lookdir;

        if (tempdir > 0)
          tempdir *= 2.f / 3.f;
        else
          tempdir /= 2.f;

        final_quat = final_quat *
                     glm::angleAxis(glm::radians(-tempdir), glm::vec3(0, 1, 0));
      }

      boneresult->rot = final_quat * boneresult->rot;

      boneresult->scale.x *= scale.x;
      boneresult->scale.y *= scale.y;
      boneresult->scale.z *= scale.z;

      boneresult->head = (final_quat) * (boneresult->head - bone->head);
      boneresult->head += bone->head;
      boneresult->head += glm::quatLookAt(glm::vec3(0, 1, 0), boneaxis) * pos;
      boneindex = bone->parent;
    }
  }
}

glm::mat4 transtomatrix(glm::vec3 pos, glm::vec3 scale, glm::quat rot) {
  glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), pos);

  glm::mat4 rotationMatrix = glm::toMat4(rot);

  glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

  return translationMatrix * rotationMatrix * scaleMatrix;
}

// renders modelgroup.
void renderModelGroup(Modeltransform* modeltrans, std::string modelgroupname,
                      bool isUI, float deltatime) {
  ModelGroupClass* modelgroup = &ModelGroupMap[modelgroupname];
  if (modeltrans->visible) {
    // code of animation frames.
    for (int i = 0; i < modeltrans->actions.size(); i++) {
      uint32_t poseindex = PosetoInt[modeltrans->actions[i].name];

      modeltrans->actions[i].frame +=
          deltatime * 24 * modeltrans->actions[i].speed;
      if ((float)modelgroup->anim[poseindex][0] ==
          (float)modelgroup->anim[poseindex][1]) {
        modeltrans->actions[i].frame = (float)modelgroup->anim[poseindex][0];
      } else {
        while (modeltrans->actions[i].frame >=
               (float)modelgroup->anim[poseindex][1])
          modeltrans->actions[i].frame +=
              ((float)modelgroup->anim[poseindex][0] -
               (float)modelgroup->anim[poseindex][1]);
        if (modeltrans->actions[i].frame <
            (float)modelgroup->anim[poseindex][0])
          modeltrans->actions[i].frame = (float)modelgroup->anim[poseindex][0];
      }
    }

    glm::vec3 lookdir(0);

    if (isUI) {
      lookdir = Camera->lookat - Camera->pos;
    } else {
      lookdir.y = 1;
    }

    if (modeltrans->modelvisibilityresult.empty()) {
      for (auto const& visiblething : modelgroup->Models) {
        modeltrans->modelvisibilityresult[visiblething] = true;
      }
    }
    if (!modeltrans->actions.empty()) {
      for (auto const& action : modeltrans->actions) {
        for (auto const& visiblething :
             modelgroup->modelvisibility[PosetoInt[action.name]]) {
          bool result = modeltrans->modelvisibilityresult[visiblething.name];
          for (auto const& [key, val] : visiblething.value) {
            if (key > action.frame) {
              break;
            } else {
              result = val;
            }
          }
          modeltrans->modelvisibilityresult[visiblething.name] = result;
        }
      }
    }

    if (!modeltrans->actions.empty())
      modelapplybones(modeltrans, PosetoInt[modeltrans->actions.back().name],
                      modelgroup, modeltrans->actions.back().frame,
                      modeltrans->lookdir.y);

    switch (Settings->graphicsmode) {
      case OpenGL4:
      case OpenGL3: {
        glm::mat4 modelMatrix;
        if (isUI) {
          modelMatrix = Global->perspectivematrix *
                        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),
                                    glm::vec3(0, 0, 1));
        } else {
          modelMatrix =
              Global->perspectivematrix *
              glm::lookAt(Camera->pos, Camera->lookat, glm::vec3(0, 0, 1));
        }

        GLuint shadertemp =
            RendererGlobal->GLstuff->GLModels.begin()->second.shader;

        glUseProgram(shadertemp);

        glUniformMatrix4fv(glGetUniformLocation(shadertemp, "model"), 1,
                           GL_FALSE, glm::value_ptr(modelMatrix));

        if (modeltrans->Bonecodevec.empty()) {
          for (const auto& [code, bone] : modelgroup->Bonemap) {
            modeltrans->Bonecodevec.push_back(code);
          }
        }
        for (int i = 0; i < modeltrans->Bonecodevec.size(); i++) {
          ModelGroupClass::Bone* bone =
              &modelgroup->Bonemap[modeltrans->Bonecodevec[i]];
          Modeltransform::BoneResult* boneresult =
              &modeltrans->Bonemap[modeltrans->Bonecodevec[i]];

          glm::mat4 resultbonemat = transtomatrix(
                        boneresult->head, boneresult->scale, boneresult->rot),
                    restmat =
                        transtomatrix(bone->restpose.pos, bone->restpose.scale,
                                      bone->restpose.rot);

          glUniform1ui(
              glGetUniformLocation(
                  shadertemp, ("bonelist[" + std::to_string(i) + "]").c_str()),
              modeltrans->Bonecodevec[i]);

          glUniformMatrix4fv(
              glGetUniformLocation(
                  shadertemp, ("restmat[" + std::to_string(i) + "]").c_str()),
              1, GL_FALSE, glm::value_ptr(restmat));

          glUniform3f(
              glGetUniformLocation(
                  shadertemp, ("bonehead[" + std::to_string(i) + "]").c_str()),
              bone->head.x, bone->head.y, bone->head.z);

          glUniformMatrix4fv(
              glGetUniformLocation(
                  shadertemp,
                  ("resultbonemat[" + std::to_string(i) + "]").c_str()),
              1, GL_FALSE, glm::value_ptr(resultbonemat));
        }

        for (const auto& modelname : modelgroup->Models) {
          if (Global->Modelmap[modelname].points.empty() ||
              !modeltrans->modelvisibilityresult[modelname])
            continue;
          uint32_t bonecode = Global->Modelmap[modelname].points.front().bone;
          Modeltransform::BoneResult* boneresult =
              &modeltrans->Bonemap[bonecode];
          ModelGroupClass::Bone* bone = &modelgroup->Bonemap[bonecode];

          glm::mat4 transformmat = transtomatrix(
              modeltrans->position, modeltrans->size, modeltrans->rot);

          glActiveTexture(GL_TEXTURE0);
          glBindTexture(GL_TEXTURE_2D,
                        RendererGlobal->GLstuff->GLModels[modelname].texture);
          glUniform1i(glGetUniformLocation(shadertemp, "InputTexture"), 0);

          glUniform1f(glGetUniformLocation(shadertemp, "lookdirx"),
                      modeltrans->lookdir.x);

          glUniformMatrix4fv(glGetUniformLocation(shadertemp, "transformmat"),
                             1, GL_FALSE, glm::value_ptr(transformmat));

          glUniform1i(glGetUniformLocation(shadertemp, "hasaction"),
                      modeltrans->actions.size());

          glBindVertexArray(
              RendererGlobal->GLstuff->GLModels[modelname].VAOthing);

          glDrawArrays(GL_TRIANGLES, 0,
                       RendererGlobal->GLstuff->GLModels[modelname].size);
        }
        break;
      }
      case OpenGL1: {  // opengl 1
        for (int a = 0; a < modelgroup->Models.size(); a++) {
          if (modeltrans->modelvisibilityresult[modelgroup->Models[a]]) {
            GlobalClass::Model* model =
                &Global->Modelmap[modelgroup->Models[a]];
            // SDL_Log("%s %d", modelgroup->Models[a].c_str(),
            //         model->faces.size());
            for (int j = 0; j < model->faces.size(); j++) {
              glEnable(GL_TEXTURE_2D);
              glBindTexture(GL_TEXTURE_2D,
                            RendererGlobal->GLstuff->textures[model->texture]);
              glBegin(GL_TRIANGLES);
              glm::vec3 tri[3];
              for (int k = 2; k >= 0; k--) {
                glm::vec3 pos = model->points[model->faces[j].point[k]].pos;

                if (!modeltrans->actions.empty()) {
                  Modeltransform::BoneResult* boneresult =
                      &modeltrans->Bonemap
                           [model->points[model->faces[j].point[k]].bone];
                  ModelGroupClass::Bone* bone =
                      &modelgroup->Bonemap
                           [model->points[model->faces[j].point[k]].bone];

                  pos.x *= bone->restpose.scale.x;
                  pos.y *= bone->restpose.scale.y;
                  pos.z *= bone->restpose.scale.z;
                  pos = bone->restpose.rot * pos;
                  pos += bone->restpose.pos;

                  pos -= bone->head;

                  pos.x *= boneresult->scale.x;
                  pos.y *= boneresult->scale.y;
                  pos.z *= boneresult->scale.z;

                  pos = boneresult->rot * pos;

                  pos += boneresult->head;
                }

                pos = glm::angleAxis(glm::radians(modeltrans->lookdir.x),
                                     glm::vec3(0, 0, 1)) *
                      pos;

                pos = modeltrans->rot * pos;
                pos.x *= modeltrans->size.x;
                pos.y *= modeltrans->size.y;
                pos.z *= modeltrans->size.z;
                // if (isUI) {
                //   pos.y *= -1;
                // }
                pos += modeltrans->position;
                tri[k] = pos;
              }

              glm::vec3 normal =
                  glm::normalize(glm::cross(tri[1] - tri[0], tri[2] - tri[0]));

              float angle = (glm::dot(normal, lookdir) + 1.f) / 3.f;
              glColor3f(angle, angle, angle);

              for (int k = 2; k >= 0; k--) {
                glTexCoord2f(model->faces[j].uv[k].x,
                             1 - model->faces[j].uv[k].y);
                glVertex3f(tri[k].x, tri[k].y, tri[k].z);
              }
              glEnd();
            }
          }
        }
        break;
      }
        // case Software: {  // software
        //   for (int j = 0; j < model->faces.size(); j++) {
        //     glm::vec3 vec[3];
        //     glm::vec2 uv[3];
        //     for (int k = 0; k < 3; k++) {
        //       glm::vec3 pos = model->points[model->faces[j].point[k]];
        //       pos.x *= Global->Models[i].size.x;
        //       pos.y *= Global->Models[i].size.y;
        //       pos.z *= Global->Models[i].size.z;
        //       pos += modeltrans->position;
        //       vec[k] = pos;
        //       uv[k] = glm::vec2(
        //           {model->faces[j].uv[k].x, 1 - model->faces[j].uv[k].y});
        //     }
        //     DrawTri(model->texture, vec, uv, 1, 1);
        //   }
        //   break;
        // }
    }
  }
}