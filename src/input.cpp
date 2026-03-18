#include "input.h"

#include <SDL3/SDL.h>

#include "extern.h"

unsigned char haspressedkey(bool keypressed, unsigned char previous) {
  if (keypressed) {
    if (previous == 0)
      return 2;
    else
      return 1;
  } else
    return 0;
}

void input() {
  const bool* key_states = SDL_GetKeyboardState(NULL);
  P1Inputs->W = haspressedkey(key_states[SDL_SCANCODE_W], P1Inputs->W);
  P1Inputs->A = haspressedkey(key_states[SDL_SCANCODE_A], P1Inputs->A);
  P1Inputs->S = haspressedkey(key_states[SDL_SCANCODE_S], P1Inputs->S);
  P1Inputs->D = haspressedkey(key_states[SDL_SCANCODE_D], P1Inputs->D);
  P1Inputs->ESC = haspressedkey(key_states[SDL_SCANCODE_ESCAPE], P1Inputs->ESC);
  P1Inputs->Shift =
      haspressedkey(key_states[SDL_SCANCODE_LSHIFT], P1Inputs->Shift);
}