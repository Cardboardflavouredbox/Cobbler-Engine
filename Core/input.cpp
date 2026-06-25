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
  LocalInputs->W = haspressedkey(key_states[SDL_SCANCODE_W], LocalInputs->W);
  LocalInputs->A = haspressedkey(key_states[SDL_SCANCODE_A], LocalInputs->A);
  LocalInputs->S = haspressedkey(key_states[SDL_SCANCODE_S], LocalInputs->S);
  LocalInputs->D = haspressedkey(key_states[SDL_SCANCODE_D], LocalInputs->D);
  LocalInputs->LCTRL =
      haspressedkey(key_states[SDL_SCANCODE_LCTRL], LocalInputs->LCTRL);
  LocalInputs->ESC =
      haspressedkey(key_states[SDL_SCANCODE_ESCAPE], LocalInputs->ESC);
  LocalInputs->Shift =
      haspressedkey(key_states[SDL_SCANCODE_LSHIFT], LocalInputs->Shift);
  LocalInputs->Space =
      haspressedkey(key_states[SDL_SCANCODE_SPACE], LocalInputs->Space);
  for (int i = 0; i < 10; i++) {
    LocalInputs->numkeys[i] =
        haspressedkey(key_states[SDL_SCANCODE_1 + i], LocalInputs->numkeys[i]);
  }
  LocalInputs->E = haspressedkey(key_states[SDL_SCANCODE_E], LocalInputs->E);
  LocalInputs->F = haspressedkey(key_states[SDL_SCANCODE_F], LocalInputs->F);
  LocalInputs->Enter =
      haspressedkey(key_states[SDL_SCANCODE_RETURN], LocalInputs->Enter);
  SDL_GetRelativeMouseState(&LocalInputs->MouseDelta.x,
                            &LocalInputs->MouseDelta.y);

  SDL_GetMouseState(&LocalInputs->MousePos.x, &LocalInputs->MousePos.y);
}