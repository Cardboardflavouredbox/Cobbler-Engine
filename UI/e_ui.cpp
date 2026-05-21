#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>

#include "extern.h"
#include "ui.h"

// UIcomponents
struct UITextChanger {
  std::string* string;
  virtual void update() = 0;
  virtual ~UITextChanger() {}
};

struct TextandNumChanger : UITextChanger {
  std::string text;
  int* num;
  void update() { *string = text + std::to_string(*num); }
  ~TextandNumChanger() {}
};

struct TextandGlobalFacePointChanger : UITextChanger {
  std::string text;
  int* num;
  int pointindex;
  void update() {
    if (*num > -1)
      *string =
          text + std::to_string(Global->mapfaces[*num].points[pointindex]);
    else
      *string = "";
  }
  ~TextandGlobalFacePointChanger() {}
};

struct TextandGlobalPointChanger : UITextChanger {
  std::string text;
  int* num;
  int xyz;  // 0==x,1==y,2==z
  void update() {
    if (*num > -1) {
      float temp;
      switch (xyz) {
        case 0:
          temp = Global->Points[*num].x;
          break;
        case 1:
          temp = Global->Points[*num].y;
          break;
        case 2:
          temp = Global->Points[*num].z;
          break;
      }
      *string = text + std::to_string(temp);
      for (int i = 0; i < 7; i++) string->pop_back();
    } else
      *string = "";
  }
  ~TextandGlobalPointChanger() {}
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
    switch (Settings->graphicsmode) {
      case 1: {
        glBegin(GL_QUADS);

        glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
        glVertex2f((pos.x - Settings->resolutionx / 2) * 2 /
                       (float)Settings->resolutionx,
                   (-pos.y + Settings->resolutiony / 2) * 2 /
                       (float)Settings->resolutiony);
        glVertex2f((pos.x + size.x - Settings->resolutionx / 2) * 2 /
                       (float)Settings->resolutionx,
                   (-pos.y + Settings->resolutiony / 2) * 2 /
                       (float)Settings->resolutiony);
        glVertex2f((pos.x + size.x - Settings->resolutionx / 2) * 2 /
                       (float)Settings->resolutionx,
                   (-pos.y - size.y + Settings->resolutiony / 2) * 2 /
                       (float)Settings->resolutiony);
        glVertex2f((pos.x - Settings->resolutionx / 2) * 2 /
                       (float)Settings->resolutionx,
                   (-pos.y - size.y + Settings->resolutiony / 2) * 2 /
                       (float)Settings->resolutiony);

        glEnd();
        break;
      }
      case 0: {
        for (int i = 0; i < size.y; i++) {
          for (int j = 0; j < size.x; j++) {
            Global->SRstuff
                ->pixelsdepth[((int)pos.x + j) +
                              ((int)pos.y + i) * Global->SRstuff->pitch] = 0;
            Global->SRstuff->pixels[((int)pos.x + j) +
                                    ((int)pos.y + i) * Global->SRstuff->pitch] =
                color;
          }
        }
        break;
      }
    }
  }
  ~UIbox() {}
};

struct UIimage : public UIthing {
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
    switch (Settings->graphicsmode) {
      case 1: {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, Global->GLstuff->textures[texturename]);
        glBegin(GL_QUADS);
        std::pair<glm::vec2, glm::vec2>* uv = &uvlist[uvindex];
        glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
        glTexCoord2f(uv->first.x, uv->first.y);
        glVertex2f((pos.x - Settings->resolutionx / 2) * 2 /
                       (float)Settings->resolutionx,
                   (-pos.y + Settings->resolutiony / 2) * 2 /
                       (float)Settings->resolutiony);
        glTexCoord2f(uv->second.x, uv->first.y);
        glVertex2f((pos.x + size.x - Settings->resolutionx / 2) * 2 /
                       (float)Settings->resolutionx,
                   (-pos.y + Settings->resolutiony / 2) * 2 /
                       (float)Settings->resolutiony);
        glTexCoord2f(uv->second.x, uv->second.y);
        glVertex2f((pos.x + size.x - Settings->resolutionx / 2) * 2 /
                       (float)Settings->resolutionx,
                   (-pos.y - size.y + Settings->resolutiony / 2) * 2 /
                       (float)Settings->resolutiony);
        glTexCoord2f(uv->first.x, uv->second.y);
        glVertex2f((pos.x - Settings->resolutionx / 2) * 2 /
                       (float)Settings->resolutionx,
                   (-pos.y - size.y + Settings->resolutiony / 2) * 2 /
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
                  ->pixelsdepth[((int)pos.x + j) +
                                ((int)pos.y + i) * Global->SRstuff->pitch] = 0;
              Global->SRstuff
                  ->pixels[((int)pos.x + j) +
                           ((int)pos.y + i) * Global->SRstuff->pitch] = color;
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
  std::string string;
  void update() {
    if (TextChanger != nullptr) {
      TextChanger->update();
    }
  }
  void render() {
    int x = pos.x, y = pos.y;
    switch (Settings->graphicsmode) {
      case 1: {  // opengl
        glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
        for (int i = 0; i < string.length(); i++) {
          if (string[i] == '\n') {
            x = pos.x;
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
            x = pos.x;
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
  void TextChangerSet(UITextChanger* newTextChanger) {
    TextChanger = newTextChanger;
    if (TextChanger != nullptr) {
      TextChanger->string = &string;
    }
  }
  ~UItext() {
    if (TextChanger != nullptr) delete (TextChanger);
  }
};
// UI setup

bool UIsetup() {
  Global->UImap.reserve(4);
  // for (int i = 0; i < 2; i++) {
  //   std::vector<std::shared_ptr<UIthing>> tempvector;
  //   std::shared_ptr<UIbox> box(std::make_shared<UIbox>());
  //   box->color = 11;
  //   box->rgba = glm::vec4(1, 1, 1, 1);
  //   box->pos = glm::vec2({4, 4});
  //   box->size = glm::vec2({64, 128});

  //   tempvector.push_back(box);

  //   std::shared_ptr<UItext> text[4] = {
  //       std::make_shared<UItext>(), std::make_shared<UItext>(),
  //       std::make_shared<UItext>(), std::make_shared<UItext>()};
  //   text[0]->color = 0;
  //   text[0]->rgba = glm::vec4(0, 0, 0, 1);
  //   text[0]->pos = glm::vec2({8, 8});
  //   text[0]->string = "test";

  //   TextandNumChanger* TaNC = new TextandNumChanger();
  //   if (i == 1) {
  //     TaNC->text = "Face";
  //     TaNC->num = &Editor->currentlyselectedface;
  //     TextandGlobalFacePointChanger* TaGFPC[3] = {
  //         new TextandGlobalFacePointChanger(),
  //         new TextandGlobalFacePointChanger(),
  //         new TextandGlobalFacePointChanger()};
  //     for (int j = 1; j < 4; j++) {
  //       text[j]->color = 0;
  //       text[j]->rgba = glm::vec4(0, 0, 0, 1);
  //       text[j]->pos = glm::vec2({8, 8 + 12.f * j});
  //       text[j]->string = "test";
  //       TaGFPC[j - 1]->text = "Point";
  //       TaGFPC[j - 1]->num = &Editor->currentlyselectedface;
  //       TaGFPC[j - 1]->pointindex = j - 1;
  //       text[j]->TextChangerSet(TaGFPC[j - 1]);
  //     }
  //   } else if (i == 0) {
  //     TaNC->text = "Point";
  //     TaNC->num = &Editor->currentlyselectedpoint;
  //     TextandGlobalPointChanger* TaGPC[3] = {new TextandGlobalPointChanger(),
  //                                            new TextandGlobalPointChanger(),
  //                                            new
  //                                            TextandGlobalPointChanger()};

  //     TaGPC[0]->text = "X ";
  //     TaGPC[0]->num = &Editor->currentlyselectedpoint;
  //     TaGPC[0]->xyz = 0;
  //     TaGPC[1]->text = "Y ";
  //     TaGPC[1]->num = &Editor->currentlyselectedpoint;
  //     TaGPC[1]->xyz = 1;
  //     TaGPC[2]->text = "Z ";
  //     TaGPC[2]->num = &Editor->currentlyselectedpoint;
  //     TaGPC[2]->xyz = 2;

  //     for (int j = 1; j < 4; j++) {
  //       text[j]->color = 0;
  //       text[j]->rgba = glm::vec4(0, 0, 0, 1);
  //       text[j]->pos = glm::vec2({8, 8 + 12.f * j});
  //       text[j]->string = "test";
  //       text[j]->TextChangerSet(TaGPC[j - 1]);
  //     }
  //   }
  //   text[0]->TextChangerSet(TaNC);

  //   for (int j = 0; j < 4; j++) {
  //     tempvector.push_back(text[j]);
  //   }

  //   if (i == 0)
  //     Global->UImap["Point"] = tempvector;
  //   else
  //     Global->UImap["Face"] = tempvector;
  // }
  return true;
}