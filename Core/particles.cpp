#include "particles.h"

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

#include "camera.h"
#include "deltaTime.h"
#include "entity.h"
#include "extern.h"
#include "pi.h"
#include "render.h"
#include "settings.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

std::random_device rd;
std::mt19937 mtwister(rd());
std::uniform_int_distribution<> randomnumber(1, 64);

struct HitParticle : Particle {
  glm::vec3 velocity = glm::vec3(0);

  void setup(uint32_t Index) {
    velocity = glm::vec3(0);
    Texture = "";
    ParticleIndex = Index;
    timeleft = 1.5f;
    float temp = 1 / 64.f;
    rect[0] = {-temp, -temp};
    rect[1] = {temp, temp};
    for (int i = 0; i < 4; i++) color[i] = 1.0f;
  }

  HitParticle(uint32_t Index) { setup(Index); }
  void update() {
    velocity.z -= 9.81f * 6.f * updatedeltaTime;
    position += direction * updatedeltaTime + velocity * updatedeltaTime;
    timeleft -= updatedeltaTime;
    if (timeleft <= 0) {
      ParticledeleteQueue.push(ParticleIndex);
    }
  }
  void lateupdate() {}
  void render() {
    switch (Settings->graphicsmode) {
      case OpenGL1: {
        float ps = std::sin(LocalPlayer->dir.x * PI / 180.f);
        float pc = std::cos(LocalPlayer->dir.x * PI / 180.f);
        float what = std::cos(LocalPlayer->dir.y * PI / 180.f);

        glm::vec3 dirthing = glm::normalize(glm::vec3{
            -ps * what, pc * what, std::sin(LocalPlayer->dir.y * PI / 180.f)});

        glm::quat quatthing =
            glm::quatLookAt(-dirthing, glm::vec3(0, 0, 1)) *
            glm::quatLookAt(glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));
        if (Texture == "") {
          glDisable(GL_TEXTURE_2D);
        } else {
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D,
                        RendererGlobal->GLstuff->textures[Texture]);
        }

        glm::vec3 quad[4];

        quad[0].x = rect[0].x;
        quad[0].y = 0;
        quad[0].z = rect[0].y;

        quad[1].x = rect[1].x;
        quad[1].y = 0;
        quad[1].z = rect[0].y;

        quad[2].x = rect[1].x;
        quad[2].y = 0;
        quad[2].z = rect[1].y;

        quad[3].x = rect[0].x;
        quad[3].y = 0;
        quad[3].z = rect[1].y;

        for (int j = 0; j < 4; j++) {
          quad[j] = quatthing * quad[j] + position;
        }

        glBegin(GL_QUADS);
        glColor4f(color[0], color[1], color[2], color[3]);
        glTexCoord2f(uv[0].x, uv[0].y);
        glVertex3f(quad[0].x, quad[0].y, quad[0].z);
        glTexCoord2f(uv[0].x + uv[1].x, uv[0].y);
        glVertex3f(quad[1].x, quad[1].y, quad[1].z);
        glTexCoord2f(uv[0].x + uv[1].x, uv[0].y + uv[1].y);
        glVertex3f(quad[2].x, quad[2].y, quad[2].z);
        glTexCoord2f(uv[0].x, uv[0].y + uv[1].y);
        glVertex3f(quad[3].x, quad[3].y, quad[3].z);
        glEnd();

        break;
      }
      case OpenGL3:
      case OpenGL4: {
        glm::mat4 modelMatrix =
            Global->perspectivematrix *
            glm::lookAt(Camera->pos, Camera->lookat, glm::vec3(0, 0, 1));

        float ps = std::sin(LocalPlayer->dir.x * PI / 180.f);
        float pc = std::cos(LocalPlayer->dir.x * PI / 180.f);
        float what = std::cos(LocalPlayer->dir.y * PI / 180.f);

        glm::vec3 dirthing = glm::normalize(glm::vec3{
            -ps * what, pc * what, std::sin(LocalPlayer->dir.y * PI / 180.f)});

        glm::quat quatthing =
            glm::quatLookAt(-dirthing, glm::vec3(0, 0, 1)) *
            glm::quatLookAt(glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));

        glm::mat4 rotationmat = glm::toMat4(quatthing);

        GLuint shadertemp = RendererGlobal->GLstuff->GLParticleBase.shader;

        glUseProgram(shadertemp);

        glUniform1i(glGetUniformLocation(shadertemp, "HasTexture"), 0);

        glUniformMatrix4fv(glGetUniformLocation(shadertemp, "particlerotation"),
                           1, GL_FALSE, glm::value_ptr(rotationmat));

        glUniformMatrix4fv(glGetUniformLocation(shadertemp, "particlemodel"), 1,
                           GL_FALSE, glm::value_ptr(modelMatrix));

        // SDL_Log("%d", glad_glGetError());
        // SDL_Log("shader %u", shadertemp);

        glUniform1i(glGetUniformLocation(shadertemp, "InputTexture"), 0);

        glUniform3f(glGetUniformLocation(shadertemp, "BillboardPosition"),
                    position.x, position.y, position.z);

        glUniform2f(glGetUniformLocation(shadertemp, "BillboardSize"),
                    rect[1].x - rect[0].x, rect[1].y - rect[0].y);

        glUniform2f(glGetUniformLocation(shadertemp, "uvOffset"), uv[0].x,
                    uv[0].y);

        glUniform2f(glGetUniformLocation(shadertemp, "uvSize"), uv[1].x,
                    uv[1].y);

        glBindVertexArray(RendererGlobal->GLstuff->GLParticleBase.VAOthing);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        break;
      }
    }
  }
};

struct ExplosionParticle : Particle {
  void setup(uint32_t Index) {
    Texture = "Explosion";
    ParticleIndex = Index;
    timeleft = 1.f;
    rect[0] = {-4.0f, -4.0f};
    rect[1] = {4.0f, 4.0f};
    uv[0] = {0, 0};
    uv[1] = {0.25f, 0.25f};
  }
  ExplosionParticle(uint32_t Index) { setup(Index); }
  void update() {
    timeleft -= updatedeltaTime;
    if (timeleft <= 0) {
      ParticledeleteQueue.push(ParticleIndex);
    }
  }
  void lateupdate() {
    int frame = int((1.f - timeleft) * 16.f);
    uv[0].x = (frame % 4) / 4.f;
    uv[0].y = (frame / 4) / 4.f;
  }
  void render() {
    switch (Settings->graphicsmode) {
      case OpenGL1: {
        float ps = std::sin(LocalPlayer->dir.x * PI / 180.f);
        float pc = std::cos(LocalPlayer->dir.x * PI / 180.f);
        float what = std::cos(LocalPlayer->dir.y * PI / 180.f);

        glm::vec3 dirthing = glm::normalize(glm::vec3{
            -ps * what, pc * what, std::sin(LocalPlayer->dir.y * PI / 180.f)});

        glm::quat quatthing =
            glm::quatLookAt(-dirthing, glm::vec3(0, 0, 1)) *
            glm::quatLookAt(glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));
        if (Texture == "") {
          glDisable(GL_TEXTURE_2D);
        } else {
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D,
                        RendererGlobal->GLstuff->textures[Texture]);
        }

        glm::vec3 quad[4];

        quad[0].x = rect[0].x;
        quad[0].y = 0;
        quad[0].z = rect[0].y;

        quad[1].x = rect[1].x;
        quad[1].y = 0;
        quad[1].z = rect[0].y;

        quad[2].x = rect[1].x;
        quad[2].y = 0;
        quad[2].z = rect[1].y;

        quad[3].x = rect[0].x;
        quad[3].y = 0;
        quad[3].z = rect[1].y;

        for (int j = 0; j < 4; j++) {
          quad[j] = quatthing * quad[j] + position;
        }

        glBegin(GL_QUADS);
        glColor4f(color[0], color[1], color[2], color[3]);
        glTexCoord2f(uv[0].x, uv[0].y);
        glVertex3f(quad[0].x, quad[0].y, quad[0].z);
        glTexCoord2f(uv[0].x + uv[1].x, uv[0].y);
        glVertex3f(quad[1].x, quad[1].y, quad[1].z);
        glTexCoord2f(uv[0].x + uv[1].x, uv[0].y + uv[1].y);
        glVertex3f(quad[2].x, quad[2].y, quad[2].z);
        glTexCoord2f(uv[0].x, uv[0].y + uv[1].y);
        glVertex3f(quad[3].x, quad[3].y, quad[3].z);
        glEnd();

        break;
      }
      case OpenGL3:
      case OpenGL4: {
        glm::mat4 modelMatrix =
            Global->perspectivematrix *
            glm::lookAt(Camera->pos, Camera->lookat, glm::vec3(0, 0, 1));

        float ps = std::sin(LocalPlayer->dir.x * PI / 180.f);
        float pc = std::cos(LocalPlayer->dir.x * PI / 180.f);
        float what = std::cos(LocalPlayer->dir.y * PI / 180.f);

        glm::vec3 dirthing = glm::normalize(glm::vec3{
            -ps * what, pc * what, std::sin(LocalPlayer->dir.y * PI / 180.f)});

        glm::quat quatthing =
            glm::quatLookAt(-dirthing, glm::vec3(0, 0, 1)) *
            glm::quatLookAt(glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));

        glm::mat4 rotationmat = glm::toMat4(quatthing);

        GLuint shadertemp = RendererGlobal->GLstuff->GLParticleBase.shader;

        glUseProgram(shadertemp);

        glUniform1i(glGetUniformLocation(shadertemp, "HasTexture"), 1);

        glUniformMatrix4fv(glGetUniformLocation(shadertemp, "particlerotation"),
                           1, GL_FALSE, glm::value_ptr(rotationmat));

        glUniformMatrix4fv(glGetUniformLocation(shadertemp, "particlemodel"), 1,
                           GL_FALSE, glm::value_ptr(modelMatrix));

        // SDL_Log("%d", glad_glGetError());
        // SDL_Log("shader %u", shadertemp);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,
                      RendererGlobal->GLstuff->textures[Texture]);

        glUniform1i(glGetUniformLocation(shadertemp, "InputTexture"), 0);

        glUniform3f(glGetUniformLocation(shadertemp, "BillboardPosition"),
                    position.x, position.y, position.z);

        glUniform2f(glGetUniformLocation(shadertemp, "BillboardSize"),
                    rect[1].x - rect[0].x, rect[1].y - rect[0].y);

        glUniform2f(glGetUniformLocation(shadertemp, "uvOffset"), uv[0].x,
                    uv[0].y);

        glUniform2f(glGetUniformLocation(shadertemp, "uvSize"), uv[1].x,
                    uv[1].y);

        glBindVertexArray(RendererGlobal->GLstuff->GLParticleBase.VAOthing);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        break;
      }
    }
  }
};

struct RayParticle : Particle {
  float length = -1, t = 0;
  glm::vec3 realposition;
  void setup(uint32_t Index) {
    t = 0;
    length = -1;
    Texture = "";
    ParticleIndex = Index;
    timeleft = -1;
  }
  RayParticle(uint32_t Index) { setup(Index); }
  void update() {
    if (SourceEntity != NULL) {
      glm::vec3 temp(0);

      if (LocalPlayer == SourceEntity) {
        temp = position;
      } else {
      }

      glm::vec3 lookat;
      lookat.x = std::cos(glm::radians(SourceEntity->dir.x + 90.f)) *
                 std::cos(glm::radians(SourceEntity->dir.y));
      lookat.z = std::sin(glm::radians(SourceEntity->dir.y));
      lookat.y = std::sin(glm::radians(SourceEntity->dir.x + 90.f)) *
                 std::cos(glm::radians(SourceEntity->dir.y));

      glm::quat rotthing = glm::quatLookAt(lookat, glm::vec3(0, 0, 1));

      temp = rotthing * temp;

      temp += SourceEntity->position;
      temp.z += SourceEntity->cameraoffset;
      realposition = temp;
    }

    if (length < 0) {
      length = glm::distance(realposition, direction);
    }
    t += 180.f * updatedeltaTime / length;
    if (t >= 1) {
      ParticledeleteQueue.push(ParticleIndex);

      glm::vec3 dirthing = -glm::normalize(direction - realposition) * 24.f;

      for (int i = 0; i < int(randomnumber(mtwister) / 8); i++) {
        glm::vec3 randomspread =
            glm::vec3(randomnumber(mtwister), randomnumber(mtwister),
                      randomnumber(mtwister)) /
            16.f;

        randomspread += dirthing;

        randomspread.z += randomnumber(mtwister) / 4.f;

        ParticleSpawn(
            ParticleSpawnInfo{2,
                              {direction.x, direction.y, direction.z},
                              {randomspread.x, randomspread.y, randomspread.z}},
            true);
      }
    }
  }
  void lateupdate() {}
  void render() {
    switch (Settings->graphicsmode) {
      case OpenGL1: {
        glm::quat quatthing =
            glm::quatLookAt(Camera->pos - realposition,
                            glm::normalize(direction - realposition));
        float t2 = t - 4.f / length;
        if (t2 < 0) t2 = 0;
        glm::vec3 temp = glm::mix(realposition, direction, t2);
        glm::vec3 temp2 = glm::mix(realposition, direction, t);

        glDisable(GL_TEXTURE_2D);
        glm::vec3 quad[4];

        for (int i = 0; i < 4; i++) {
          quad[i].y = 0;
          quad[i].z = 0;
          if (i % 2 == 0) {
            quad[i].x = -0.0625f / 16.f;
          } else {
            quad[i].x = 0.0625f / 16.f;
          }
          quad[i] = quatthing * quad[i];
          if (i / 2 == 0) {
            quad[i] += temp;
          } else {
            quad[i] += temp2;
          }
        }

        glBegin(GL_QUADS);
        glColor4f(1, 1, 1, 1);
        glVertex3f(quad[0].x, quad[0].y, quad[0].z);
        glVertex3f(quad[1].x, quad[1].y, quad[1].z);
        glVertex3f(quad[2].x, quad[2].y, quad[2].z);
        glVertex3f(quad[3].x, quad[3].y, quad[3].z);
        glEnd();

        break;
      }
      case OpenGL3:
      case OpenGL4: {
        glm::mat4 modelMatrix =
            Global->perspectivematrix *
            glm::lookAt(Camera->pos, Camera->lookat, glm::vec3(0, 0, 1));

        glm::quat quatthing =
            glm::quatLookAt(Camera->pos - realposition,
                            glm::normalize(direction - realposition));

        glm::mat4 rotationmat = glm::toMat4(quatthing);

        GLuint shadertemp = RendererGlobal->GLstuff->shaders["Ray"];

        glUseProgram(shadertemp);

        glUniformMatrix4fv(glGetUniformLocation(shadertemp, "rotation"), 1,
                           GL_FALSE, glm::value_ptr(rotationmat));

        glUniformMatrix4fv(glGetUniformLocation(shadertemp, "model"), 1,
                           GL_FALSE, glm::value_ptr(modelMatrix));

        // SDL_Log("%d", glad_glGetError());
        // SDL_Log("shader %u", shadertemp);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,
                      RendererGlobal->GLstuff->textures[Texture]);

        float t2 = t - 4.f / length;
        if (t2 < 0) t2 = 0;

        glm::vec3 temp = glm::mix(realposition, direction, t2);
        glm::vec3 temp2 = glm::mix(realposition, direction, t);

        glUniform3f(glGetUniformLocation(shadertemp, "origin"), temp.x, temp.y,
                    temp.z);

        glUniform3f(glGetUniformLocation(shadertemp, "ray"), temp2.x, temp2.y,
                    temp2.z);

        glUniform1f(glGetUniformLocation(shadertemp, "size"), 0.0625f / 8.f);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        break;
      }
    }
  }
};

Particle* NewParticle(uint32_t Particlecode, uint32_t Index) {
  switch (Particlecode) {
    case 2:
      return new HitParticle(Index);
      break;
    case 1:
      return new RayParticle(Index);
      break;
    default:
      return new ExplosionParticle(Index);
  }
}