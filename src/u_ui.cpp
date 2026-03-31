#include "extern.h"
#include "ui.h"

void UIbox::update() {}
void UIbox::render() {
  for (int i = 0; i < size.y; i++) {
    for (int j = 0; j < size.x; j++) {
      Global->pixelsdepth[((int)pos.x + j) + ((int)pos.y + i) * Global->pitch] =
          0;
      Global->pixels[((int)pos.x + j) + ((int)pos.y + i) * Global->pitch] =
          color;
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
          Global->pixelsdepth[(k + x) +
                              (j + y + 12 - glyph.offsety) * Global->pitch] = 0;
          Global
              ->pixels[(k + x) + (j + y + 12 - glyph.offsety) * Global->pitch] =
              color;
        }
      }
    }
    x += glyph.advancex;
    y += glyph.advancey;
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
  *string = text + std::to_string(Global->mapfaces[*num].points[pointindex]);
}

void TextandGlobalPointChanger::update() {
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
}