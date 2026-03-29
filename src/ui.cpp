#include "ui.h"

#include <SDL3/SDL.h>

// void UIthing::render(unsigned char* pixels, unsigned char pixelsdepth[],
//                      int pitch) {
//   SDL_Log("default");
// }

void UIbox::update() {}
void UIbox::render(unsigned char* pixels, unsigned char pixelsdepth[],
                   int pitch) {
  SDL_Log("UIbox");
  for (int i = 0; i < size.y; i++) {
    for (int j = 0; j < size.x; j++) {
      pixelsdepth[((int)pos.x + j) + ((int)pos.y + i) * pitch] = 0;
      pixels[((int)pos.x + j) + ((int)pos.y + i) * pitch] = color;
    }
  }
}