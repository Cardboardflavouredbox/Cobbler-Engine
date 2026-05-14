#include "extern.h"
#include "ui.h"

void UIbox::update() {}
void UIbox::render() {
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

void UIimage::update() {
  if (UVIndexChanger != nullptr) {
    UVIndexChanger->update();
  }
}
void UIimage::render() {
  switch (Settings->graphicsmode) {
    case 1: {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, Global->GLstuff->textures[textureindex]);
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
      SDL_Surface* surface = Global->SRstuff->textures[textureindex];
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
            Global->SRstuff->pixels[((int)pos.x + j) +
                                    ((int)pos.y + i) * Global->SRstuff->pitch] =
                color;
          }
        }
      }
      break;
    }
  }
}

void UItext::update() {
  if (TextChanger != nullptr) {
    TextChanger->update();
  }
}
void UItext::render() {
  int x = pos.x, y = pos.y;
  switch (Settings->graphicsmode) {
    case 1: {  // opengl
      glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
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

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, Global->Glyphmap[temp].GLTexture);
        glBegin(GL_QUADS);

        glTexCoord2f(0, 0);
        glVertex2f(
            (x - Settings->resolutionx / 2) * 2 / (float)Settings->resolutionx,
            (-y + Settings->resolutiony / 2 - 12 + glyph.offsety) * 2 /
                (float)Settings->resolutiony);
        glTexCoord2f(1, 0);
        glVertex2f((x + x2 - Settings->resolutionx / 2) * 2 /
                       (float)Settings->resolutionx,
                   (-y + Settings->resolutiony / 2 - 12 + glyph.offsety) * 2 /
                       (float)Settings->resolutiony);
        glTexCoord2f(1, 1);
        glVertex2f((x + x2 - Settings->resolutionx / 2) * 2 /
                       (float)Settings->resolutionx,
                   (-y - y2 + Settings->resolutiony / 2 - 12 + glyph.offsety) *
                       2 / (float)Settings->resolutiony);
        glTexCoord2f(0, 1);
        glVertex2f(
            (x - Settings->resolutionx / 2) * 2 / (float)Settings->resolutionx,
            (-y - y2 + Settings->resolutiony / 2 - 12 + glyph.offsety) * 2 /
                (float)Settings->resolutiony);

        glEnd();

        x += glyph.advancex;
        y += glyph.advancey;
      }
      break;
    }
    case 0: {  // software
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
                ((glyph.pixels[k / 8 + j * glyph.pitch]) >> (7 - k % 8) &
                 0x01)) {
              Global->SRstuff
                  ->pixelsdepth[(k + x) + (j + y + 12 - glyph.offsety) *
                                              Global->SRstuff->pitch] = 0;
              Global->SRstuff->pixels[(k + x) + (j + y + 12 - glyph.offsety) *
                                                    Global->SRstuff->pitch] =
                  color;
            }
          }
        }
        x += glyph.advancex;
        y += glyph.advancey;
      }
      break;
    }
  }
}

void UItext::TextChangerSet(UITextChanger* newTextChanger) {
  TextChanger = newTextChanger;
  if (TextChanger != nullptr) {
    TextChanger->string = &string;
  }
}

void TextandNumChanger::update() { *string = text + std::to_string(*num); }

void TextandGlobalFacePointChanger::update() {
  if (*num > -1)
    *string = text + std::to_string(Global->mapfaces[*num].points[pointindex]);
  else
    *string = "";
}

void TextandGlobalPointChanger::update() {
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

void ImagePistolChanger::update() {
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