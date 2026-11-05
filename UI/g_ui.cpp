#include "g_ui.h"

#include <SDL3/SDL_log.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>

#include "camera.h"
#include "commandline.h"
#include "commandlineui.h"
#include "damageindicator.h"
#include "inputs.h"
#include "pausemenu.h"
#include "render.h"
#include "screen.h"
#include "ui.h"

#ifdef _WIN32
#ifdef UIEXPORT
#define UI_API __declspec(dllexport)
#else
#define UI_API __declspec(dllimport)
#endif
#else
#define UI_API
#endif

void UIbox::render() {
  glm::vec2 realpos = getrealpos(anchor, pos, size);
  switch (Settings->graphicsmode) {
    case OpenGL3:
    case OpenGL4: {
      GLuint shadertemp = RendererGlobal->GLstuff->shaders["UI"];
      glUseProgram(shadertemp);
      glBindVertexArray(RendererGlobal->GLstuff->emptyVAO);

      // glActiveTexture(GL_TEXTURE0);
      // glBindTexture(GL_TEXTURE_2D,
      //               RendererGlobal->GLstuff->GLModels[modelname].texture);

      glUniform2i(glGetUniformLocation(shadertemp, "screensize"),
                  Settings->resolutionx, Settings->resolutiony);
      glUniform2f(glGetUniformLocation(shadertemp, "rotationoffset"), 0, 0);
      glUniform1f(glGetUniformLocation(shadertemp, "rotateangle"), 0);
      glUniform2f(glGetUniformLocation(shadertemp, "position"), realpos.x,
                  realpos.y);
      glUniform2f(glGetUniformLocation(shadertemp, "size"), size.x, size.y);
      // glUniform2f(glGetUniformLocation(shadertemp, "uvpos"), realpos.x,
      //             realpos.y);
      // glUniform2f(glGetUniformLocation(shadertemp, "uvsize"), realpos.x,
      //             realpos.y);
      glUniform1i(glGetUniformLocation(shadertemp, "textureexists"), 0);
      // glUniform1i(glGetUniformLocation(shadertemp, "TextureInput"), 0);
      glUniform4f(glGetUniformLocation(shadertemp, "baseColor"), rgba.x, rgba.y,
                  rgba.z, rgba.w);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      break;
    }
    case OpenGL1: {
      glBegin(GL_QUADS);

      glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
      glVertex2f((realpos.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);
      glVertex2f((realpos.x + size.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);
      glVertex2f((realpos.x + size.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y - size.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);
      glVertex2f((realpos.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y - size.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);

      glEnd();
      break;
    }
    case Software: {
      for (int i = 0; i < size.y; i++) {
        for (int j = 0; j < size.x; j++) {
          RendererGlobal->SRstuff
              ->pixelsdepth[((int)realpos.x + j) +
                            ((int)realpos.y + i) *
                                RendererGlobal->SRstuff->pitch] = 0;
          RendererGlobal->SRstuff
              ->pixels[((int)realpos.x + j) +
                       ((int)realpos.y + i) * RendererGlobal->SRstuff->pitch] =
              color;
        }
      }
      break;
    }
  }
}

void UITextInputBox::render() {
  glm::vec2 realpos = getrealpos(anchor, pos, size);
  switch (Settings->graphicsmode) {
    case OpenGL3:
    case OpenGL4: {
      GLuint shadertemp = RendererGlobal->GLstuff->shaders["UI"];
      glUseProgram(shadertemp);
      glBindVertexArray(RendererGlobal->GLstuff->emptyVAO);

      // glActiveTexture(GL_TEXTURE0);
      // glBindTexture(GL_TEXTURE_2D,
      //               RendererGlobal->GLstuff->GLModels[modelname].texture);

      glUniform2i(glGetUniformLocation(shadertemp, "screensize"),
                  Settings->resolutionx, Settings->resolutiony);
      glUniform2f(glGetUniformLocation(shadertemp, "rotationoffset"), 0, 0);
      glUniform1f(glGetUniformLocation(shadertemp, "rotateangle"), 0);
      glUniform2f(glGetUniformLocation(shadertemp, "position"), realpos.x,
                  realpos.y);
      glUniform2f(glGetUniformLocation(shadertemp, "size"), size.x, size.y);
      // glUniform2f(glGetUniformLocation(shadertemp, "uvpos"), realpos.x,
      //             realpos.y);
      // glUniform2f(glGetUniformLocation(shadertemp, "uvsize"), realpos.x,
      //             realpos.y);
      glUniform1i(glGetUniformLocation(shadertemp, "textureexists"), 0);
      // glUniform1i(glGetUniformLocation(shadertemp, "TextureInput"), 0);
      glUniform4f(glGetUniformLocation(shadertemp, "baseColor"), rgba.x, rgba.y,
                  rgba.z, rgba.w);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      break;
    }
    case OpenGL1: {
      glBegin(GL_QUADS);

      glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
      glVertex2f((realpos.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);
      glVertex2f((realpos.x + size.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);
      glVertex2f((realpos.x + size.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y - size.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);
      glVertex2f((realpos.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y - size.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);

      glEnd();
      break;
    }
    case Software: {
      for (int i = 0; i < size.y; i++) {
        for (int j = 0; j < size.x; j++) {
          RendererGlobal->SRstuff
              ->pixelsdepth[((int)realpos.x + j) +
                            ((int)realpos.y + i) *
                                RendererGlobal->SRstuff->pitch] = 0;
          RendererGlobal->SRstuff
              ->pixels[((int)realpos.x + j) +
                       ((int)realpos.y + i) * RendererGlobal->SRstuff->pitch] =
              color;
        }
      }
      break;
    }
  }
}

void UITextInputBox::update() {
  glm::vec2 realpos = getrealpos(anchor, pos, size);
  if (LocalInputs->leftclick > 1) {
    float w = RendererGlobal->windowx, h = RendererGlobal->windowy,
          rtw = Settings->resolutionx, rth = Settings->resolutiony;
    float sizetemp = w / rtw;
    if (sizetemp > h / rth) sizetemp = h / rth;

    realpos *= sizetemp;

    if (realpos.x < LocalInputs->MousePos.x &&
        realpos.x + size.x * sizetemp > LocalInputs->MousePos.x &&
        realpos.y < LocalInputs->MousePos.y &&
        realpos.y + size.y * sizetemp > LocalInputs->MousePos.y) {
      textinputing = true;
    } else {
      textinputing = false;
    }
  }
}

void UIimage::render() {
  glm::vec2 realpos = getrealpos(anchor, pos, size);
  switch (Settings->graphicsmode) {
    case OpenGL3:
    case OpenGL4: {
      GLuint shadertemp = RendererGlobal->GLstuff->shaders["UI"];
      glUseProgram(shadertemp);
      glBindVertexArray(RendererGlobal->GLstuff->emptyVAO);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,
                    RendererGlobal->GLstuff->textures[texturename]);
      glUniform1i(glGetUniformLocation(shadertemp, "TextureInput"), 0);

      glUniform2i(glGetUniformLocation(shadertemp, "screensize"),
                  Settings->resolutionx, Settings->resolutiony);

      glUniform2f(glGetUniformLocation(shadertemp, "rotationoffset"), 0, 0);
      glUniform1f(glGetUniformLocation(shadertemp, "rotateangle"), 0);
      glUniform2f(glGetUniformLocation(shadertemp, "position"), realpos.x,
                  realpos.y);
      glUniform2f(glGetUniformLocation(shadertemp, "size"), size.x, size.y);
      glUniform2f(glGetUniformLocation(shadertemp, "uvpos"), uvlist->first.x,
                  uvlist->first.y);
      glUniform2f(glGetUniformLocation(shadertemp, "uvsize"), uvlist->second.x,
                  uvlist->second.y);
      glUniform1i(glGetUniformLocation(shadertemp, "textureexists"), 0);
      glUniform1i(glGetUniformLocation(shadertemp, "textureexists"), 0);
      glUniform4f(glGetUniformLocation(shadertemp, "baseColor"), rgba.x, rgba.y,
                  rgba.z, rgba.w);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      break;
    }
    case OpenGL1: {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,
                    RendererGlobal->GLstuff->textures[texturename]);
      glBegin(GL_QUADS);
      std::pair<glm::vec2, glm::vec2>* uv = &uvlist[uvindex];
      glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
      glTexCoord2f(uv->first.x, uv->first.y);
      glVertex2f((realpos.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);
      glTexCoord2f(uv->second.x, uv->first.y);
      glVertex2f((realpos.x + size.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);
      glTexCoord2f(uv->second.x, uv->second.y);
      glVertex2f((realpos.x + size.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y - size.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);
      glTexCoord2f(uv->first.x, uv->second.y);
      glVertex2f((realpos.x - Settings->resolutionx / 2) * 2 /
                     (float)Settings->resolutionx,
                 (-realpos.y - size.y + Settings->resolutiony / 2) * 2 /
                     (float)Settings->resolutiony);

      glEnd();
      break;
    }
    case Software: {
      std::pair<glm::vec2, glm::vec2> uv = uvlist[uvindex];
      SDL_Surface* surface = RendererGlobal->SRstuff->textures[texturename];
      uv.first.x *= surface->w;
      uv.first.y *= surface->h;
      uv.second.x *= surface->w;
      uv.second.y *= surface->h;
      for (int i = 0; i < size.y; i++) {
        for (int j = 0; j < size.x; j++) {
          uint8_t color = static_cast<uint8_t*>(surface->pixels)[int(
              uv.first.x + j + surface->pitch * (uv.first.y + i))];
          if (color > 0) {
            RendererGlobal->SRstuff
                ->pixelsdepth[((int)realpos.x + j) +
                              ((int)realpos.y + i) *
                                  RendererGlobal->SRstuff->pitch] = 0;
            RendererGlobal->SRstuff
                ->pixels[((int)realpos.x + j) +
                         ((int)realpos.y + i) *
                             RendererGlobal->SRstuff->pitch] = color;
          }
        }
      }
      break;
    }
  }
}

void UItext::render() {
  glm::vec2 realpos = getrealpos(anchor, pos, glm::vec2(0, 0));
  int x = realpos.x, y = realpos.y;
  switch (Settings->graphicsmode) {
    case OpenGL3:
    case OpenGL4: {
      if (stringstartfromright) {
        for (int i = string.length() - 1; i >= 0; i--) {
          if (string[i] == '\n') {
            x = realpos.x;
            y -= 12 * size;
          } else {
            FT_UInt temp = FT_Get_Char_Index(Freetypething->FTface, string[i]);
            if (!Freetypething->Glyphmap.contains(temp)) {
              FT_Load_Glyph(Freetypething->FTface, temp, FT_LOAD_MONOCHROME);
              FT_Render_Glyph(Freetypething->FTface->glyph,
                              FT_RENDER_MODE_MONO);
              Freetypething->Glyphmap[temp] =
                  CreateGlyph(Freetypething->FTface->glyph);
            }
            CustomGlyphthing glyph = Freetypething->Glyphmap[temp];
            x += glyph.offsetx * size;
            int x2 = glyph.width * size, y2 = glyph.height * size;

            x -= glyph.advancex * size;
            y -= glyph.advancey * size;

            GLuint shadertemp = RendererGlobal->GLstuff->shaders["UI"];
            glUseProgram(shadertemp);
            glBindVertexArray(RendererGlobal->GLstuff->emptyVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,
                          Freetypething->Glyphmap[temp].GLTexture);
            glUniform1i(glGetUniformLocation(shadertemp, "TextureInput"), 0);

            glUniform2i(glGetUniformLocation(shadertemp, "screensize"),
                        Settings->resolutionx, Settings->resolutiony);

            glUniform2f(glGetUniformLocation(shadertemp, "rotationoffset"), 0,
                        0);
            glUniform1f(glGetUniformLocation(shadertemp, "rotateangle"), 0);
            glUniform2f(glGetUniformLocation(shadertemp, "position"), x,
                        y + 12 - glyph.offsety);
            glUniform2f(glGetUniformLocation(shadertemp, "size"), x2, y2);
            glUniform2f(glGetUniformLocation(shadertemp, "uvpos"), 0, 0);
            glUniform2f(glGetUniformLocation(shadertemp, "uvsize"), 1, 1);
            glUniform1i(glGetUniformLocation(shadertemp, "textureexists"), 1);
            glUniform4f(glGetUniformLocation(shadertemp, "baseColor"), rgba.x,
                        rgba.y, rgba.z, rgba.w);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
          }
        }
      } else {
        for (int i = 0; i < string.length(); i++) {
          if (string[i] == '\n') {
            x = realpos.x;
            y += 12 * size;
          } else {
            FT_UInt temp = FT_Get_Char_Index(Freetypething->FTface, string[i]);
            if (!Freetypething->Glyphmap.contains(temp)) {
              FT_Load_Glyph(Freetypething->FTface, temp, FT_LOAD_MONOCHROME);
              FT_Render_Glyph(Freetypething->FTface->glyph,
                              FT_RENDER_MODE_MONO);
              Freetypething->Glyphmap[temp] =
                  CreateGlyph(Freetypething->FTface->glyph);
            }
            CustomGlyphthing glyph = Freetypething->Glyphmap[temp];
            x += glyph.offsetx * size;
            int x2 = glyph.width * size, y2 = glyph.height * size;

            GLuint shadertemp = RendererGlobal->GLstuff->shaders["UI"];
            glUseProgram(shadertemp);
            glBindVertexArray(RendererGlobal->GLstuff->emptyVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,
                          Freetypething->Glyphmap[temp].GLTexture);
            glUniform1i(glGetUniformLocation(shadertemp, "TextureInput"), 0);

            glUniform2f(glGetUniformLocation(shadertemp, "rotationoffset"), 0,
                        0);
            glUniform1f(glGetUniformLocation(shadertemp, "rotateangle"), 0);
            glUniform2f(glGetUniformLocation(shadertemp, "position"), x,
                        y + 12 - glyph.offsety);
            glUniform2f(glGetUniformLocation(shadertemp, "size"), x2, y2);
            glUniform2f(glGetUniformLocation(shadertemp, "uvpos"), 0, 0);
            glUniform2f(glGetUniformLocation(shadertemp, "uvsize"), 1, 1);
            glUniform1i(glGetUniformLocation(shadertemp, "textureexists"), 1);
            glUniform4f(glGetUniformLocation(shadertemp, "baseColor"), rgba.x,
                        rgba.y, rgba.z, rgba.w);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            x += glyph.advancex * size;
            y += glyph.advancey * size;
          }
        }
      }
      break;
    }
    case OpenGL1: {
      glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
      if (stringstartfromright) {
        for (int i = string.length() - 1; i >= 0; i--) {
          if (string[i] == '\n') {
            x = realpos.x;
            y -= 12 * size;
          } else {
            FT_UInt temp = FT_Get_Char_Index(Freetypething->FTface, string[i]);
            if (!Freetypething->Glyphmap.contains(temp)) {
              FT_Load_Glyph(Freetypething->FTface, temp, FT_LOAD_MONOCHROME);
              FT_Render_Glyph(Freetypething->FTface->glyph,
                              FT_RENDER_MODE_MONO);
              Freetypething->Glyphmap[temp] =
                  CreateGlyph(Freetypething->FTface->glyph);
            }
            CustomGlyphthing glyph = Freetypething->Glyphmap[temp];
            x += glyph.offsetx * size;
            int x2 = glyph.width * size, y2 = glyph.height * size;

            x -= glyph.advancex * size;
            y -= glyph.advancey * size;

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,
                          Freetypething->Glyphmap[temp].GLTexture);
            glBegin(GL_QUADS);

            glTexCoord2f(0, 0);
            glVertex2f((x - Settings->resolutionx / 2) * 2 /
                           (float)Settings->resolutionx,
                       (-y + Settings->resolutiony / 2 - 12 + glyph.offsety) *
                           2 / (float)Settings->resolutiony);
            glTexCoord2f(1, 0);
            glVertex2f((x + x2 - Settings->resolutionx / 2) * 2 /
                           (float)Settings->resolutionx,
                       (-y + Settings->resolutiony / 2 - 12 + glyph.offsety) *
                           2 / (float)Settings->resolutiony);
            glTexCoord2f(1, 1);
            glVertex2f(
                (x + x2 - Settings->resolutionx / 2) * 2 /
                    (float)Settings->resolutionx,
                (-y - y2 + Settings->resolutiony / 2 - 12 + glyph.offsety) * 2 /
                    (float)Settings->resolutiony);
            glTexCoord2f(0, 1);
            glVertex2f(
                (x - Settings->resolutionx / 2) * 2 /
                    (float)Settings->resolutionx,
                (-y - y2 + Settings->resolutiony / 2 - 12 + glyph.offsety) * 2 /
                    (float)Settings->resolutiony);

            glEnd();
          }
        }
      } else {
        for (int i = 0; i < string.length(); i++) {
          if (string[i] == '\n') {
            x = realpos.x;
            y += 12 * size;
          } else {
            FT_UInt temp = FT_Get_Char_Index(Freetypething->FTface, string[i]);
            if (!Freetypething->Glyphmap.contains(temp)) {
              FT_Load_Glyph(Freetypething->FTface, temp, FT_LOAD_MONOCHROME);
              FT_Render_Glyph(Freetypething->FTface->glyph,
                              FT_RENDER_MODE_MONO);
              Freetypething->Glyphmap[temp] =
                  CreateGlyph(Freetypething->FTface->glyph);
            }
            CustomGlyphthing glyph = Freetypething->Glyphmap[temp];
            x += glyph.offsetx * size;
            int x2 = glyph.width * size, y2 = glyph.height * size;

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,
                          Freetypething->Glyphmap[temp].GLTexture);
            glBegin(GL_QUADS);

            glTexCoord2f(0, 0);
            glVertex2f((x - Settings->resolutionx / 2) * 2 /
                           (float)Settings->resolutionx,
                       (-y + Settings->resolutiony / 2 - 12 + glyph.offsety) *
                           2 / (float)Settings->resolutiony);
            glTexCoord2f(1, 0);
            glVertex2f((x + x2 - Settings->resolutionx / 2) * 2 /
                           (float)Settings->resolutionx,
                       (-y + Settings->resolutiony / 2 - 12 + glyph.offsety) *
                           2 / (float)Settings->resolutiony);
            glTexCoord2f(1, 1);
            glVertex2f(
                (x + x2 - Settings->resolutionx / 2) * 2 /
                    (float)Settings->resolutionx,
                (-y - y2 + Settings->resolutiony / 2 - 12 + glyph.offsety) * 2 /
                    (float)Settings->resolutiony);
            glTexCoord2f(0, 1);
            glVertex2f(
                (x - Settings->resolutionx / 2) * 2 /
                    (float)Settings->resolutionx,
                (-y - y2 + Settings->resolutiony / 2 - 12 + glyph.offsety) * 2 /
                    (float)Settings->resolutiony);

            glEnd();

            x += glyph.advancex * size;
            y += glyph.advancey * size;
          }
        }
      }
      break;
    }
    case Software: {
      for (int i = 0; i < string.length(); i++) {
        if (string[i] == '\n') {
          x = realpos.x;
          y += 12;
        } else {
          FT_UInt temp = FT_Get_Char_Index(Freetypething->FTface, string[i]);
          if (!Freetypething->Glyphmap.contains(temp)) {
            FT_Load_Glyph(Freetypething->FTface, temp, FT_LOAD_MONOCHROME);
            FT_Render_Glyph(Freetypething->FTface->glyph, FT_RENDER_MODE_MONO);
            Freetypething->Glyphmap[temp] =
                CreateGlyph(Freetypething->FTface->glyph);
          }
          CustomGlyphthing glyph = Freetypething->Glyphmap[temp];
          x += glyph.offsetx;
          int x2 = glyph.width, y2 = glyph.height;
          for (int j = 0; j < y2; j++) {
            for (int k = 0; k < x2; k++) {
              if (k > -1 && k < Settings->resolutionx && j > -1 &&
                  j < Settings->resolutiony &&
                  ((glyph.pixels[k / 8 + j * glyph.pitch]) >> (7 - k % 8) &
                   0x01)) {
                RendererGlobal->SRstuff
                    ->pixelsdepth[(k + x) +
                                  (j + y + 12 - glyph.offsety) *
                                      RendererGlobal->SRstuff->pitch] = 0;
                RendererGlobal->SRstuff
                    ->pixels[(k + x) + (j + y + 12 - glyph.offsety) *
                                           RendererGlobal->SRstuff->pitch] =
                    color;
              }
            }
          }
          x += glyph.advancex;
          y += glyph.advancey;
        }
      }
      break;
    }
  }
}

void UIDamageIndicator::render() {
  glm::vec2 realpos = getrealpos(anchor, pos, glm::vec2(0, 0));
  switch (Settings->graphicsmode) {
    case OpenGL3:
    case OpenGL4: {
      for (auto& i : LocalDamageLocations) {
        glm::vec3 temppos;
        for (int j = 0; j < 3; j++) temppos[j] = i.position[j];
        glm::vec2 dirresult = ToScreenSpace(temppos).p;
        dirresult.x -= Settings->resolutionx / 2;
        dirresult.y -= Settings->resolutiony / 2;
        dirresult = glm::normalize(dirresult);

        GLuint shadertemp = RendererGlobal->GLstuff->shaders["UI"];
        glUseProgram(shadertemp);
        glBindVertexArray(RendererGlobal->GLstuff->emptyVAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,
                      RendererGlobal->GLstuff->textures[texturename]);
        glUniform1i(glGetUniformLocation(shadertemp, "TextureInput"), 0);

        glUniform2i(glGetUniformLocation(shadertemp, "screensize"),
                    Settings->resolutionx, Settings->resolutiony);
        glUniform2f(glGetUniformLocation(shadertemp, "position"), realpos.x,
                    realpos.y);
        glUniform2f(glGetUniformLocation(shadertemp, "size"), size.x, size.y);
        glUniform2f(glGetUniformLocation(shadertemp, "uvpos"), 0, 0);
        glUniform2f(glGetUniformLocation(shadertemp, "uvsize"), 1, 1);
        glUniform2f(glGetUniformLocation(shadertemp, "rotationoffset"), -32, 0);
        glUniform1f(glGetUniformLocation(shadertemp, "rotateangle"),
                    -atan2f(dirresult.y, -dirresult.x));
        glUniform1i(glGetUniformLocation(shadertemp, "textureexists"), 0);
        glUniform4f(glGetUniformLocation(shadertemp, "baseColor"), 1, 0, 0,
                    i.lifetime / i.lifestart);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      }
      break;
    }
  }
}

void firstpersonUIthing::update() {
  float temp = playerluastuff->lua["state"];
  int state = int(temp);

  auto result = playerluastuff->LocalClassStatetoPose();
  uint32_t animationthing = getposeindex(result);

  float anim = playerluastuff->lua["anim"];

  modeltrans->actions[0].frame =
      anim + ModelGroupMap["PlayerMain"].anim[animationthing][0];
  if (currentstate != state) {
    modeltrans->actions[0].pose.posecode.front() = animationthing;
    currentstate = state;
  }
}

glm::vec2 getrealpos(UIthing::anchorpos anchor, glm::vec2 pos, glm::vec2 size) {
  glm::vec2 realpos;
  switch (anchor % 3) {
    case 0:
      realpos.x = pos.x;
      break;
    case 1:
      realpos.x = Settings->resolutionx / 2 - size.x / 2 + pos.x;
      break;
    case 2:
      realpos.x = Settings->resolutionx + pos.x;
      break;
  }
  switch (anchor / 3) {
    case 0:
      realpos.y = pos.y;
      break;
    case 1:
      realpos.y = Settings->resolutiony / 2 - size.y / 2 + pos.y;
      break;
    case 2:
      realpos.y = Settings->resolutiony + pos.y;
      break;
  }
  return realpos;
}

void clearMenuOptionsVector() {
  MenuOptionsVector.clear();
  if (UIGlobalStuff->UImap.contains("Menus")) {
    while (!UIGlobalStuff->UImap["Menus"].empty()) {
      delete (UIGlobalStuff->UImap["Menus"].back());
      UIGlobalStuff->UImap["Menus"].pop_back();
    }
    UIGlobalStuff->UImap.erase("Menus");
  }
  while (UIGlobalStuff->UIlist.back() == "Menus")
    UIGlobalStuff->UIlist.pop_back();
}

// UI setup
bool UIsetup() {
  UIGlobalStuff->UImap.reserve(4);
  UIGlobalStuff->UImap3D.reserve(4);

  UIGlobalStuff->UImap["Pause"];
  UIGlobalStuff->UImap3D["Pause"];
  UIbox* background = new UIbox();
  background->anchor = UIthing::lefttop;
  background->rgba = glm::vec4(0, 0, 0, 0.5f);
  background->color = 255;
  background->size = glm::vec2({Settings->resolutionx, Settings->resolutiony});
  background->pos = glm::vec2({0, 0});
  UIGlobalStuff->UImap["Pause"].push_back(background);

  UIbox* cmdborder = new UIbox();
  cmdborder->anchor = UIthing::righttop;
  cmdborder->rgba = glm::vec4(1, 1, 1, 1.0f);
  cmdborder->color = 0;
  cmdborder->size = glm::vec2({366, 292});
  cmdborder->pos = glm::vec2({-375, 9});
  UIGlobalStuff->UImap["Pause"].push_back(cmdborder);

  UIbox* cmdbox = new UIbox();
  cmdbox->anchor = UIthing::righttop;
  cmdbox->rgba = glm::vec4(0, 0, 0, 1.0f);
  cmdbox->color = 255;
  cmdbox->size = glm::vec2({360, 270});
  cmdbox->pos = glm::vec2({-372, 12});
  UIGlobalStuff->UImap["Pause"].push_back(cmdbox);

  UITextInputBox* cmdinputbox = new UITextInputBox();
  cmdinputbox->anchor = UIthing::righttop;
  cmdinputbox->rgba = glm::vec4(0, 0, 0, 1.0f);
  cmdinputbox->color = 255;
  cmdinputbox->size = glm::vec2({360, 12});
  cmdinputbox->pos = glm::vec2({-372, 286});
  UIGlobalStuff->UImap["Pause"].push_back(cmdinputbox);

  MenuCMDtextChanger* cmdtextchanger = new MenuCMDtextChanger();
  UItext* cmdtext = new UItext(cmdtextchanger);
  cmdtext->stringstartfromright = false;
  cmdtext->size = 1;
  cmdtext->anchor = UIthing::righttop;
  cmdtext->rgba = glm::vec4(1, 1, 1, 1);
  cmdtext->pos = glm::vec2(-372, 12);
  UIGlobalStuff->UImap["Pause"].push_back(cmdtext);

  MenuCMDInputtextChanger* cmdinputtextchanger = new MenuCMDInputtextChanger();
  UItext* cmdinputtext = new UItext(cmdinputtextchanger);
  cmdinputtext->stringstartfromright = false;
  cmdinputtext->size = 1;
  cmdinputtext->anchor = UIthing::righttop;
  cmdinputtext->rgba = glm::vec4(1, 1, 1, 1);
  cmdinputtext->pos = glm::vec2(-372, 286);
  UIGlobalStuff->UImap["Pause"].push_back(cmdinputtext);

  UIGlobalStuff->UImap["CobblerGameBaseUI"];

  UIbox* hpbox = new UIbox();
  hpbox->anchor = UIthing::leftbottom;
  hpbox->size = glm::vec2({80, 48});
  hpbox->rgba = glm::vec4(0, 0, 0, 0.75f);
  hpbox->pos = glm::vec2(12, -60);
  UIGlobalStuff->UImap["CobblerGameBaseUI"].push_back(hpbox);

  HPTextChanger* textnum = new HPTextChanger();
  UItext* hptext = new UItext(textnum);
  hptext->stringstartfromright = true;
  hptext->size = 3;
  hptext->anchor = UIthing::leftbottom;
  hptext->rgba = glm::vec4(1, 1, 1, 1);
  hptext->pos = glm::vec2(84, -52);
  UIGlobalStuff->UImap["CobblerGameBaseUI"].push_back(hptext);

  UIDamageIndicator* DIndicator = new UIDamageIndicator();
  DIndicator->pos = glm::vec2(-12, -4);
  DIndicator->size = glm::vec2(24, 8);
  UIGlobalStuff->UImap["CobblerGameBaseUI"].push_back(DIndicator);

  // GardnerStuff
  UIGlobalStuff->UImap["Gardner/AmmoCnt"];

  UIbox* ammobox = new UIbox();
  ammobox->anchor = UIthing::rightbottom;
  ammobox->size = glm::vec2({80, 48});
  ammobox->rgba = glm::vec4(0, 0, 0, 0.75f);
  ammobox->pos = glm::vec2(-92, -60);
  UIGlobalStuff->UImap["Gardner/AmmoCnt"].push_back(ammobox);

  UIbox* ammobox2 = new UIbox();
  ammobox2->anchor = UIthing::rightbottom;
  ammobox2->size = glm::vec2({64, 24});
  ammobox2->rgba = glm::vec4(0, 0, 0, 0.75f);
  ammobox2->pos = glm::vec2(-76, -84);
  UIGlobalStuff->UImap["Gardner/AmmoCnt"].push_back(ammobox2);

  AmmoTextChanger* ammotextnum = new AmmoTextChanger();
  UItext* ammotext = new UItext(ammotextnum);
  ammotext->stringstartfromright = true;
  ammotext->size = 3;
  ammotext->anchor = UIthing::rightbottom;
  ammotext->rgba = glm::vec4(1, 1, 1, 1);
  ammotext->pos = glm::vec2(-16, -52);
  UIGlobalStuff->UImap["Gardner/AmmoCnt"].push_back(ammotext);

  ReserveTextChanger* reservetextnum = new ReserveTextChanger();
  UItext* reservetext = new UItext(reservetextnum);
  reservetext->stringstartfromright = true;
  reservetext->size = 2;
  reservetext->anchor = UIthing::rightbottom;
  reservetext->rgba = glm::vec4(1, 1, 1, 1);
  reservetext->pos = glm::vec2(-16, -80);
  UIGlobalStuff->UImap["Gardner/AmmoCnt"].push_back(reservetext);

  UIGlobalStuff->UImap["Gardner/Weapon0"];
  UIGlobalStuff->UImap3D["Gardner/Weapon0"];

  UIbox* crosshair = new UIbox();
  crosshair->anchor = UIthing::middlemiddle;
  crosshair->rgba = glm::vec4(1, 1, 1, 1);
  crosshair->color = 11;
  crosshair->size = glm::vec2({2, 2});
  crosshair->pos = glm::vec2({0, 0});

  UIGlobalStuff->UImap["Gardner/Weapon0"].push_back(crosshair);

  UI3Dthing* ArmUI = new firstpersonUIthing();

  UIGlobalStuff->UImap3D["Gardner/Weapon0"].push_back(ArmUI);

  return true;
}

void UIfree() { clearMenuOptionsVector(); }