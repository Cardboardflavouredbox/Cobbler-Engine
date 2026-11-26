#include "render.h"

#include <SDL3/SDL_log.h>

#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/quaternion.hpp>
#include <utility>

#include "camera.h"
#include "extern.h"
#include "lights.h"
#include "pi.h"
#include "rendermath.h"
#include "screen.h"
#include "settings.h"

// function that turns vec3 into 2d point on screen.
// for software rendering.
ScreenPoint ToScreenSpace(glm::vec3 P) {
  glm::mat4 view = glm::lookAt(Camera->pos, Camera->lookat, glm::vec3(0, 0, 1));
  glm::vec4 viewport(0.0f, 0.0f, Settings->resolutionx, Settings->resolutiony);

  glm::vec3 result = glm::project(P, view, Global->perspectivematrix, viewport);

  ScreenPoint screenpos;
  if (result.z <= 0.25f) {
    screenpos.isbehindLocalPlayer = true;
  }
  screenpos.p = result;
  screenpos.dist = 1.f / result.z;
  return screenpos;
}

struct posesresult {
  glm::vec3 pos = glm::vec3(0), scale = glm::vec3(1);
  glm::quat rot = glm::quat(1, 0, 0, 0);
};

posesresult posesmapprocess(uint32_t bonecode, uint32_t actioncode,
                            float frame) {
  posesresult result;

  if (AnimMap[actioncode].Poses.contains(bonecode)) {
    auto& posespos = AnimMap[actioncode].Poses[bonecode].pos;
    auto& posesscale = AnimMap[actioncode].Poses[bonecode].scale;
    auto& posesrot = AnimMap[actioncode].Poses[bonecode].rot;

    if (!posespos.empty()) {
      auto it = posespos.lower_bound((uint32_t)std::ceilf(frame));

      if (it == posespos.end()) {
        result.pos = posespos.rbegin()->second;
      } else if (it->first == frame || it == posespos.begin()) {
        result.pos = it->second;
      } else {
        auto prev_it = std::prev(it);

        float next_frame = it->first;
        float prev_frame = prev_it->first;

        float a = (frame - prev_frame) / (next_frame - prev_frame);

        result.pos = glm::mix(prev_it->second, it->second, a);
      }
    }

    if (!posesscale.empty()) {
      auto it = posesscale.lower_bound((uint32_t)std::ceilf(frame));

      if (it == posesscale.end()) {
        result.scale = posesscale.rbegin()->second;
      } else if (it->first == frame || it == posesscale.begin()) {
        result.scale = it->second;
      } else {
        auto prev_it = std::prev(it);

        float next_frame = it->first;
        float prev_frame = prev_it->first;

        float a = (frame - prev_frame) / (next_frame - prev_frame);

        result.scale = glm::mix(prev_it->second, it->second, a);
      }
    }

    if (!posesrot.empty()) {
      auto it = posesrot.lower_bound((uint32_t)std::ceilf(frame));

      if (it == posesrot.end()) {
        result.rot = posesrot.rbegin()->second;
      } else if (it->first == frame || it == posesrot.begin()) {
        result.rot = it->second;
      } else {
        auto prev_it = std::prev(it);

        float next_frame = it->first;
        float prev_frame = prev_it->first;

        float a = (frame - prev_frame) / (next_frame - prev_frame);

        glm::quat tempquat1 = (prev_it->second), tempquat2 = (it->second);

        if (glm::dot(tempquat1, tempquat2) < 0.0f) {
          tempquat2 = -tempquat2;
        }
        tempquat1 = (tempquat1);
        tempquat2 = (tempquat2);

        glm::quat tempresult = glm::mix(tempquat1, tempquat2, a);

        result.rot = tempresult;
      }
    }
  }
  return result;
}

struct actionapplystruct {
  std::array<uint32_t, 2> actioncode;
  float lerpamount = 0.f, frame = 0.f;
};

BoneResult applyboneactions(const uint32_t& bonecode,
                            const ModelGroupClass::Bone& inputbone,
                            std::vector<actionapplystruct> actions,
                            ModelGroupClass* modelgroup) {
  uint32_t boneindex = bonecode;
  BoneResult boneresult;
  boneresult.head = inputbone.head;
  boneresult.rot = glm::quat(1, 0, 0, 0);
  boneresult.scale = glm::vec3(1);
  // apply bones
  while (boneindex != uint32_t(-1)) {
    ModelGroupClass::Bone* bone = &modelgroup->Bonemap[boneindex];

    posesresult poseresultthing;

    for (auto& action : actions) {
      float frame = action.frame;

      if (Settings->crunchyanimations) frame = std::floorf(frame);

      posesresult poseresultthing2 =
                      posesmapprocess(boneindex, action.actioncode[0], frame),
                  poseresultthing3 =
                      posesmapprocess(boneindex, action.actioncode[1], frame);

      poseresultthing.pos += glm::mix(poseresultthing2.pos,
                                      poseresultthing3.pos, action.lerpamount);

      poseresultthing.scale *= glm::mix(
          poseresultthing2.scale, poseresultthing3.scale, action.lerpamount);

      poseresultthing.rot = poseresultthing.rot * glm::mix(poseresultthing2.rot,
                                                           poseresultthing3.rot,
                                                           action.lerpamount);
    }

    // the axis translation code of fear and despair...
    float angle = glm::angle(poseresultthing.rot);

    glm::vec3 baseaxis = glm::axis(poseresultthing.rot);

    glm::vec3 boneaxis = glm::normalize(bone->tail - bone->head);

    glm::quat axisquat = glm::quatLookAt(glm::vec3(0, 1, 0), boneaxis);

    glm::vec3 axis = axisquat * baseaxis;

    glm::quat final_quat;

    final_quat = glm::normalize(glm::angleAxis(angle, axis));

    glm::quat scalething = glm::rotation(glm::vec3(0, 1, 0), axis);

    poseresultthing.scale = glm::vec3(1.0f) +
                            glm::abs(axisquat * glm::vec3(1, 0, 0)) *
                                (poseresultthing.scale.x - 1.0f) +
                            glm::abs(axisquat * glm::vec3(0, 1, 0)) *
                                (poseresultthing.scale.y - 1.0f) +
                            glm::abs(axisquat * glm::vec3(0, 0, 1)) *
                                (poseresultthing.scale.z - 1.0f);

    if (boneindex == bonecode) {
      if (poseresultthing.pos != glm::vec3(0) ||
          poseresultthing.scale != glm::vec3(1) ||
          final_quat != glm::quat(1, 0, 0, 0))
        boneresult.check = true;
      boneresult.scale = poseresultthing.scale;
    }

    boneresult.rot = final_quat * boneresult.rot;

    boneresult.head = (final_quat) * ((boneresult.head - bone->head));
    boneresult.head += poseresultthing.scale * (bone->tail - bone->head) -
                       bone->tail + bone->head * 2.f;
    boneresult.head += axisquat * (poseresultthing.pos * bone->restpose.scale);

    if (AnimMap[actions.front().actioncode[0]].links.contains(boneindex))
      boneindex = AnimMap[actions.front().actioncode[0]].links[boneindex];
    else
      boneindex = bone->parent;
  }
  return boneresult;
}

// apply animations of bones.
void modelapplybones(Modeltransform* modeltrans, ModelGroupClass* modelgroup) {
  for (auto& [code, bone] : modelgroup->Bonemap) {
    BoneResult* boneresult = &BoneResultmapVector[code];
    boneresult->head = bone.head;
    boneresult->rot = glm::quat(1, 0, 0, 0);
    boneresult->scale = glm::vec3(1);
  }

  std::vector<actionapplystruct> actions;

  for (auto& action : modeltrans->actions) {
    auto& actionposething = action.pose;

    actionapplystruct actionresult;

    if (actionposething.lerpamount == std::floorf(actionposething.lerpamount)) {
      actionresult.actioncode[0] =
          actionposething.posecode[int(actionposething.lerpamount)];
      actionresult.actioncode[1] = actionresult.actioncode[0];
    } else {
      actionresult.actioncode[0] =
          actionposething.posecode[int(actionposething.lerpamount)];
      actionresult.actioncode[1] =
          actionposething.posecode[int(actionposething.lerpamount) + 1];
      actionresult.lerpamount =
          actionposething.lerpamount - std::floorf(actionposething.lerpamount);
    }

    actionresult.frame = action.frame;

    actions.push_back(actionresult);
  }

  for (auto& [code, bone] : modelgroup->Bonemap) {
    BoneResultmapVector[code] =
        applyboneactions(code, bone, actions, modelgroup);
  }
}

glm::mat4 transtomatrix(const glm::vec3& pos, const glm::vec3& scale,
                        const glm::quat& rot) {
  glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), pos);

  glm::mat4 rotationMatrix = glm::toMat4(rot);

  glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

  return translationMatrix * rotationMatrix * scaleMatrix;
}

// renders modelgroup.
void renderModelGroup(Modeltransform* modeltrans,
                      const std::vector<std::string>& modelgroupname, bool isUI,
                      float deltatime) {
  for (int i = 0; i < modeltrans->actions.size(); i++) {
    uint32_t poseindex = modeltrans->actions[i].pose.posecode.front();

    modeltrans->actions[i].frame +=
        deltatime * 24 * modeltrans->actions[i].speed;
    if ((float)AnimMap[poseindex].animstartend[0] ==
        (float)AnimMap[poseindex].animstartend[1]) {
      modeltrans->actions[i].frame = (float)AnimMap[poseindex].animstartend[0];
    } else {
      modeltrans->actions[i].actiondone =
          (modeltrans->actions[i].frame >=
           (float)AnimMap[poseindex].animstartend[1]);
    }
  }
  BoneResultmapVector.clear();
  for (const auto& namething : modelgroupname) {
    if (ModelGroupMap.contains(namething)) {
      ModelGroupClass* modelgroup = &ModelGroupMap[namething];
      if (modeltrans->visible) {
        // code of animation frames.

        glm::vec3 lookdir(0);

        if (isUI) {
          lookdir = Camera->lookat - Camera->pos;
        } else {
          lookdir.y = 1;
        }

        modelapplybones(modeltrans, modelgroup);

        switch (Settings->graphicsmode) {
          case OpenGL4:
          case OpenGL3: {
            GLuint shadertemp =
                RendererGlobal->GLstuff->GLModels.begin()->second.shader;

            glUseProgram(shadertemp);

            glm::mat4 modelMatrix, UImatrix;
            if (isUI) {
              modelMatrix = Global->perspectivematrix *
                            glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),
                                        glm::vec3(0, 0, 1));
              UImatrix = transtomatrix(
                  Camera->pos, glm::vec3(1),
                  glm::lookAt(Camera->pos, Camera->lookat, glm::vec3(0, 0, 1)));

              glUniform3f(glGetUniformLocation(shadertemp, "viewPos"), 0, 0, 0);
            } else {
              modelMatrix =
                  Global->perspectivematrix *
                  glm::lookAt(Camera->pos, Camera->lookat, glm::vec3(0, 0, 1));

              glUniform3f(glGetUniformLocation(shadertemp, "viewPos"),
                          Camera->pos.x, Camera->pos.y, Camera->pos.z);
            }

            glUniformMatrix4fv(
                glGetUniformLocation(shadertemp, "UIreversemodel"), 1, GL_FALSE,
                glm::value_ptr(UImatrix));

            glUniformMatrix4fv(glGetUniformLocation(shadertemp, "model"), 1,
                               GL_FALSE, glm::value_ptr(modelMatrix));

            glUniform1f(glGetUniformLocation(shadertemp, "ambientStrength"),
                        AmbientStrength);
            glUniform3f(glGetUniformLocation(shadertemp, "ambientColor"),
                        AmbientColor[0], AmbientColor[1], AmbientColor[2]);

            glUniform1i(glGetUniformLocation(shadertemp, "IsUI"), isUI);

            glUniform1i(glGetUniformLocation(shadertemp, "lightcnt"),
                        Lights.size());

            int cnt = 0;
            for (auto& [index, light] : Lights) {
              glUniform3f(
                  glGetUniformLocation(
                      shadertemp,
                      ("lightPos[" + std::to_string(cnt) + "]").c_str()),
                  light->position.x, light->position.y, light->position.z);
              glUniform3f(
                  glGetUniformLocation(
                      shadertemp,
                      ("lightColor[" + std::to_string(cnt) + "]").c_str()),
                  light->color[0], light->color[1], light->color[2]);
              glUniform1f(glGetUniformLocation(
                              shadertemp,
                              ("specularStrength[" + std::to_string(cnt) + "]")
                                  .c_str()),
                          light->specularStrength);
              glUniform1f(glGetUniformLocation(
                              shadertemp,
                              ("diffusionStrength[" + std::to_string(cnt) + "]")
                                  .c_str()),
                          light->diffusionStrength);
              cnt++;
            }
            std::vector<uint32_t> Bonecodevec;

            for (const auto& [code, bone] : modelgroup->Bonemap) {
              Bonecodevec.push_back(code);
            }

            int actionsize = Bonecodevec.size();

            glUniform1i(glGetUniformLocation(shadertemp, "actionsize"),
                        actionsize);

            for (int i = 0; i < actionsize; i++) {
              ModelGroupClass::Bone* bone =
                  &modelgroup->Bonemap[Bonecodevec[i]];

              glm::mat4 restmat = transtomatrix(
                  bone->restpose.pos, bone->restpose.scale, bone->restpose.rot);

              BoneResult* boneresult = &BoneResultmapVector[Bonecodevec[i]];

              int index = i;

              glUniform1ui(
                  glGetUniformLocation(
                      shadertemp,
                      ("bonelist[" + std::to_string(index) + "]").c_str()),
                  Bonecodevec[i]);

              glUniform1i(glGetUniformLocation(
                              shadertemp,
                              ("resultbonecheck[" + std::to_string(index) + "]")
                                  .c_str()),
                          (boneresult->check) ? 1 : 0);

              glUniformMatrix4fv(
                  glGetUniformLocation(
                      shadertemp,
                      ("restmat[" + std::to_string(index) + "]").c_str()),
                  1, GL_FALSE, glm::value_ptr(restmat));

              glUniform3f(
                  glGetUniformLocation(
                      shadertemp,
                      ("bonehead[" + std::to_string(index) + "]").c_str()),
                  bone->head.x, bone->head.y, bone->head.z);

              glUniform3f(
                  glGetUniformLocation(
                      shadertemp,
                      ("resultbonepos[" + std::to_string(index) + "]").c_str()),
                  boneresult->head.x, boneresult->head.y, boneresult->head.z);

              glUniform3f(glGetUniformLocation(
                              shadertemp,
                              ("resultbonescale[" + std::to_string(index) + "]")
                                  .c_str()),
                          boneresult->scale.x, boneresult->scale.y,
                          boneresult->scale.z);

              glUniform4f(glGetUniformLocation(
                              shadertemp,
                              ("resultbonequat[" + std::to_string(index) + "]")
                                  .c_str()),
                          boneresult->rot.x, boneresult->rot.y,
                          boneresult->rot.z, boneresult->rot.w);
            }

            for (const auto& modelname : modelgroup->Models) {
              if (Global->Modelmap[modelname].points.empty()) continue;
              uint32_t bonecode =
                  Global->Modelmap[modelname].points.front().bone;

              glm::mat4 transformmat = transtomatrix(
                  modeltrans->position, modeltrans->size, modeltrans->rot);

              glActiveTexture(GL_TEXTURE0);
              glBindTexture(
                  GL_TEXTURE_2D,
                  RendererGlobal->GLstuff->GLModels[modelname].texture);
              glUniform1i(glGetUniformLocation(shadertemp, "InputTexture"), 0);

              glUniform1f(glGetUniformLocation(shadertemp, "lookdirx"),
                          modeltrans->lookdir.x);

              glUniformMatrix4fv(
                  glGetUniformLocation(shadertemp, "transformmat"), 1, GL_FALSE,
                  glm::value_ptr(transformmat));

              glBindVertexArray(
                  RendererGlobal->GLstuff->GLModels[modelname].VAOthing);

              glDrawArrays(GL_TRIANGLES, 0,
                           RendererGlobal->GLstuff->GLModels[modelname].size);
            }
            break;
          }
          case OpenGL1: {  // opengl 1
            for (int a = 0; a < modelgroup->Models.size(); a++) {
              if (true) {
                GlobalClass::Model* model =
                    &Global->Modelmap[modelgroup->Models[a]];

                for (int j = 0; j < model->faces.size(); j++) {
                  glEnable(GL_TEXTURE_2D);
                  glBindTexture(
                      GL_TEXTURE_2D,
                      RendererGlobal->GLstuff->textures[model->texture]);
                  int facesize = model->faces[j].point.size();
                  if (facesize == 4) {
                    glBegin(GL_QUADS);
                  } else {
                    glBegin(GL_TRIANGLES);
                  }
                  glm::vec3 tri[4];

                  for (int k = facesize - 1; k >= 0; k--) {
                    glm::vec3 pos = model->points[model->faces[j].point[k]].pos;

                    uint32_t bonecode =
                        model->points[model->faces[j].point[k]].bone;

                    bonecode = model->points[model->faces[j].point[k]].bone;

                    BoneResult* boneresult = &BoneResultmapVector[bonecode];

                    ModelGroupClass::Bone* bone =
                        &modelgroup->Bonemap[bonecode];

                    pos *= bone->restpose.scale;
                    pos = bone->restpose.rot * pos;
                    pos += bone->restpose.pos;

                    pos -= bone->head;

                    pos = boneresult->rot * pos;
                    pos *= boneresult->scale;

                    pos += boneresult->head;

                    pos = glm::angleAxis(glm::radians(modeltrans->lookdir.x),
                                         glm::vec3(0, 0, 1)) *
                          pos;

                    pos *= modeltrans->size;
                    pos = modeltrans->rot * pos;

                    // if (isUI) {
                    //   pos.y *= -1;
                    // }
                    pos += modeltrans->position;
                    tri[k] = pos;
                  }

                  glm::vec3 normal = glm::normalize(
                      glm::cross(tri[1] - tri[0], tri[2] - tri[0]));

                  float angle = (glm::dot(normal, lookdir) + 1.f) / 3.f + 0.25f;
                  glColor3f(angle, angle, angle);

                  for (int k = facesize - 1; k >= 0; k--) {
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
            //           {model->faces[j].uv[k].x, 1 -
            //           model->faces[j].uv[k].y});
            //     }
            //     DrawTri(model->texture, vec, uv, 1, 1);
            //   }
            //   break;
            // }
        }
      }
    }
  }
}