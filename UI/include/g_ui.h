#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>

#include "extern.h"
#include "pausemenu.h"
#include "ui.h"

glm::vec2 getrealpos(UIthing::anchorpos anchor, glm::vec2 pos, glm::vec2 size);

// UIcomponents
struct UITextChanger {
  std::u32string* string;
  virtual void update() = 0;
  virtual ~UITextChanger() {}
};

struct TextandNumChanger : UITextChanger {
  std::u32string text;
  int* num;
  void update() {
    std::string s = std::to_string(*num);
    std::u32string s32(s.begin(), s.end());
    *string = text + s32;
  }
  ~TextandNumChanger() {}
};

struct MenuOptionTextChanger : UITextChanger {
  int* index;
  std::u32string* optionname;
  void update() { *string = (*index == menuindex ? U">" : U" ") + *optionname; }
  MenuOptionTextChanger() {}
  ~MenuOptionTextChanger() {}
};

struct MenuCheckboxTextChanger : UITextChanger {
  int* index;
  bool* enabled;
  std::u32string* optionname;
  void update() {
    *string = (*index == menuindex ? U">" : U" ") + *optionname +
              (*enabled ? U"▣" : U"□");
  }
  MenuCheckboxTextChanger() {}
  ~MenuCheckboxTextChanger() {}
};

struct UIImageUVIndexChanger {
  int* index;
  virtual void update() = 0;
  virtual ~UIImageUVIndexChanger() {}
};

struct ImagePistolChanger : UIImageUVIndexChanger {
  float animlen[3] = {0.f, 0.5f, 2.f}, animprogress = 0.f;
  unsigned char anim = 0;
  void update() {
    if (anim == 0) {
      if (P1Inputs->leftclick > 1) {
        anim = 1;
        animprogress = animlen[1];
      }
    } else {
      animprogress -= Global->deltaTime;
      if (animprogress <= 0) {
        if (anim == 1) {
          anim = 2;
          animprogress = animlen[2] + animprogress;
        } else {
          anim = 0;
        }
      }
    }
    *index = anim;
  }
  ~ImagePistolChanger() {}
};

// UIthings
struct UIbox : public UIthing {
  unsigned char color;
  glm::vec4 rgba;
  glm::vec2 size;
  void update() {}
  void render() {
    glm::vec2 realpos = getrealpos(anchor, pos, size);
    switch (Settings->graphicsmode) {
      case 1: {
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
      case 0: {
        for (int i = 0; i < size.y; i++) {
          for (int j = 0; j < size.x; j++) {
            Global->SRstuff
                ->pixelsdepth[((int)realpos.x + j) +
                              ((int)realpos.y + i) * Global->SRstuff->pitch] =
                0;
            Global->SRstuff
                ->pixels[((int)realpos.x + j) +
                         ((int)realpos.y + i) * Global->SRstuff->pitch] = color;
          }
        }
        break;
      }
    }
  }
  ~UIbox() {}
};

struct UIimage : public UIthing {  // turn stuff into shared pointers!!
  unsigned char color;
  glm::vec4 rgba;
  UIImageUVIndexChanger* UVIndexChanger = nullptr;
  glm::vec2 size;
  std::pair<glm::vec2, glm::vec2>* uvlist;
  std::string texturename;
  int uvindex = 0;
  void update() {
    if (UVIndexChanger != nullptr) {
      UVIndexChanger->update();
    }
  }
  void render() {
    glm::vec2 realpos = getrealpos(anchor, pos, size);
    switch (Settings->graphicsmode) {
      case 1: {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, Global->GLstuff->textures[texturename]);
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
      case 0: {
        std::pair<glm::vec2, glm::vec2> uv = uvlist[uvindex];
        SDL_Surface* surface = Global->SRstuff->textures[texturename];
        uv.first.x *= surface->w;
        uv.first.y *= surface->h;
        uv.second.x *= surface->w;
        uv.second.y *= surface->h;
        for (int i = 0; i < size.y; i++) {
          for (int j = 0; j < size.x; j++) {
            Uint8 color = static_cast<Uint8*>(surface->pixels)[int(
                uv.first.x + j + surface->pitch * (uv.first.y + i))];
            if (color > 0) {
              Global->SRstuff
                  ->pixelsdepth[((int)realpos.x + j) +
                                ((int)realpos.y + i) * Global->SRstuff->pitch] =
                  0;
              Global->SRstuff
                  ->pixels[((int)realpos.x + j) +
                           ((int)realpos.y + i) * Global->SRstuff->pitch] =
                  color;
            }
          }
        }
        break;
      }
    }
  }
  ~UIimage() {
    if (UVIndexChanger != nullptr) delete (UVIndexChanger);
    delete (uvlist);
  }
};

struct UItext : public UIthing {
  unsigned char color;
  glm::vec4 rgba;
  UITextChanger* TextChanger = nullptr;
  std::u32string string;
  void update() {
    if (TextChanger != nullptr) {
      TextChanger->update();
    }
  }
  void render() {
    glm::vec2 realpos = getrealpos(anchor, pos, glm::vec2(0, 0));
    int x = realpos.x, y = realpos.y;
    switch (Settings->graphicsmode) {
      case 1: {  // opengl
        glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
        for (int i = 0; i < string.length(); i++) {
          if (string[i] == '\n') {
            x = realpos.x;
            y += 12;
          } else {
            FT_UInt temp = FT_Get_Char_Index(Global->FTface, string[i]);
            if (!Global->Glyphmap.contains(temp)) {
              FT_Load_Glyph(Global->FTface, temp, FT_LOAD_MONOCHROME);
              FT_Render_Glyph(Global->FTface->glyph, FT_RENDER_MODE_MONO);
              Global->Glyphmap[temp] = CreateGlyph(Global->FTface->glyph);
            }
            CustomGlyphthing glyph = Global->Glyphmap[temp];
            x += glyph.offsetx;
            int x2 = glyph.width, y2 = glyph.height;

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, Global->Glyphmap[temp].GLTexture);
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

            x += glyph.advancex;
            y += glyph.advancey;
          }
        }
        break;
      }
      case 0: {  // software
        for (int i = 0; i < string.length(); i++) {
          if (string[i] == '\n') {
            x = realpos.x;
            y += 12;
          } else {
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
                    ((glyph.pixels[k / 8 + j * glyph.pitch]) >> (7 - k % 8) &
                     0x01)) {
                  Global->SRstuff
                      ->pixelsdepth[(k + x) + (j + y + 12 - glyph.offsety) *
                                                  Global->SRstuff->pitch] = 0;
                  Global->SRstuff
                      ->pixels[(k + x) + (j + y + 12 - glyph.offsety) *
                                             Global->SRstuff->pitch] = color;
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
  UItext(UITextChanger* newTextChanger) {
    TextChanger = newTextChanger;
    if (TextChanger != nullptr) {
      TextChanger->string = &string;
    }
  }
  ~UItext() {
    if (TextChanger != nullptr) {
      delete (TextChanger);
    }
  }
};
