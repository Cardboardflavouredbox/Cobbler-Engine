#include <ft2build.h>
#include <glad/glad.h>

#include <unordered_map>
#include FT_FREETYPE_H

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

struct CustomGlyphthing {
  unsigned char* pixels;
  GLuint GLTexture;
  unsigned int width, height;
  int pitch, offsetx, offsety, advancex, advancey;
};

extern "C" {
LIB_API CustomGlyphthing CreateGlyph(FT_GlyphSlot glyph);
}

struct FreetypeClass {
  FT_Library FTlibrary;
  FT_Face FTface;
  std::unordered_map<uint32_t, CustomGlyphthing> Glyphmap;
};

LIB_API extern FreetypeClass* Freetypething;