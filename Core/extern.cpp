#include "extern.h"

#include <SDL3/SDL_timer.h>

std::unique_ptr<GlobalClass> Global;
std::unique_ptr<EditorClass> Editor;
std::unique_ptr<SettingsClass> Settings;
Inputs* P1Inputs;
std::shared_ptr<Entity> Camera;
Uint64 lastTime;
Uint64 currentTime = SDL_GetPerformanceCounter();
std::unordered_map<std::string, std::shared_ptr<Entity> (*)()> SpawnEntities;

CustomGlyphthing CreateGlyph(FT_GlyphSlot glyph) {
  CustomGlyphthing temp;
  temp.width = glyph->bitmap.width;
  temp.height = glyph->bitmap.rows;
  temp.pitch = glyph->bitmap.pitch;
  temp.advancex = glyph->advance.x / 64;
  temp.advancey = glyph->advance.y / 64;
  temp.offsetx = glyph->bitmap_left;
  temp.offsety = glyph->bitmap_top;

  switch (Settings->graphicsmode) {
    case 1: {
      glGenTextures(1, &temp.GLTexture);
      temp.pixels = new unsigned char[8 * temp.pitch * temp.height]();
      for (int i = 0; i < 8 * temp.pitch * temp.height; i++) {
        temp.pixels[i] =
            (glyph->bitmap.buffer[i / 8] & (0x01 << (7 - i % 8))) ? 255 : 0;
      }

      glBindTexture(GL_TEXTURE_2D, temp.GLTexture);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, temp.width, temp.height, 0,
                   GL_ALPHA, GL_UNSIGNED_BYTE, temp.pixels);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      break;
    }
    case 0: {
      temp.pixels = new unsigned char[temp.width * temp.height];
      for (int i = 0; i < temp.width * temp.height; i++) {
        temp.pixels[i] = glyph->bitmap.buffer[i];
      }
      break;
    }
  }

  return temp;
}
