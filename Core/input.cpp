#include <SDL3/SDL_mouse.h>

#include "inputs.h"
#include "update.h"

// key press function.
// 2 == pressed this frame.
// 1 == still being pressed after 2.
// 0 == not being pressed at all.
unsigned char haspressedkey(bool keypressed, unsigned char previous) {
  if (keypressed) {
    if (previous == 0) return 2;
    return 1;
  }
  return 0;
}

// input processing function.
void input() {
  // get Mouse data.
  SDL_GetRelativeMouseState(&LocalInputs->MouseDelta.x,
                            &LocalInputs->MouseDelta.y);
  SDL_GetMouseState(&LocalInputs->MousePos.x, &LocalInputs->MousePos.y);
}