#include <SDL3/SDL_mouse.h>

#include "inputs.h"
#include "update.h"

// input processing function.
void input() {
  // get Mouse data.
  SDL_GetRelativeMouseState(&LocalInputs->MouseDelta.x,
                            &LocalInputs->MouseDelta.y);
  SDL_GetMouseState(&LocalInputs->MousePos.x, &LocalInputs->MousePos.y);
}