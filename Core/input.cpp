#include "extern.h"
#include "update.h"

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
  P1Inputs->LCTRL =
      haspressedkey(key_states[SDL_SCANCODE_LCTRL], P1Inputs->LCTRL);
  P1Inputs->ESC = haspressedkey(key_states[SDL_SCANCODE_ESCAPE], P1Inputs->ESC);
  P1Inputs->Shift =
      haspressedkey(key_states[SDL_SCANCODE_LSHIFT], P1Inputs->Shift);
  P1Inputs->Space =
      haspressedkey(key_states[SDL_SCANCODE_SPACE], P1Inputs->Space);
  for (int i = 0; i < 10; i++) {
    P1Inputs->numkeys[i] =
        haspressedkey(key_states[SDL_SCANCODE_1 + i], P1Inputs->numkeys[i]);
  }
  P1Inputs->E = haspressedkey(key_states[SDL_SCANCODE_E], P1Inputs->E);
  P1Inputs->F = haspressedkey(key_states[SDL_SCANCODE_F], P1Inputs->F);
  P1Inputs->Enter =
      haspressedkey(key_states[SDL_SCANCODE_KP_ENTER], P1Inputs->Enter);
  SDL_GetRelativeMouseState(&P1Inputs->MouseDelta.x, &P1Inputs->MouseDelta.y);
  SDL_GetMouseState(&P1Inputs->MousePos.x, &P1Inputs->MousePos.y);
}