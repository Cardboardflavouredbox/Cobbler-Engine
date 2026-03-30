#include "ui.h"

#include "extern.h"
// void UIthing::render(unsigned char* pixels, unsigned char pixelsdepth[],
//                      int pitch) {
//   SDL_Log("default");
// }

void UIbox::update() {}
void UIbox::render(unsigned char* pixels, unsigned char pixelsdepth[],
                   int pitch) {
  for (int i = 0; i < size.y; i++) {
    for (int j = 0; j < size.x; j++) {
      pixelsdepth[((int)pos.x + j) + ((int)pos.y + i) * pitch] = 0;
      pixels[((int)pos.x + j) + ((int)pos.y + i) * pitch] = color;
    }
  }
}

void UItext::update() {}
void UItext::render(unsigned char* pixels, unsigned char pixelsdepth[],
                    int pitch) {
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
          pixelsdepth[(k + x) + (j + y + 12 - glyph.offsety) * pitch] = 0;
          pixels[(k + x) + (j + y + 12 - glyph.offsety) * pitch] = color;
        }
      }
    }
    x += glyph.advancex;
    y += glyph.advancey;
  }
}
