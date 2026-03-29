#include "ui.h"
void UIbox::update() {}
void UIbox::render(unsigned char* pixels, unsigned char pixelsdepth[],
                   int pitch) {
  for (int i = 0; i < size.y; i++) {
    for (int j = 0; j < size.x; j++) {
      pixelsdepth[j + i * pitch] = 0;
      pixels[j + i * pitch] = color;
    }
  }
}