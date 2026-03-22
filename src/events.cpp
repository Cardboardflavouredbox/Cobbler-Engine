#include <SDL3/SDL.h>

#include "extern.h"
#include "update.h"
void events() {
  if (P1Inputs->leftclick == 2) P1Inputs->leftclick = 1;
  if (P1Inputs->rightclick == 2) P1Inputs->rightclick = 1;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        Global->IsRunning = false;
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          P1Inputs->leftclick = 2;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          P1Inputs->rightclick = 2;
        }
        break;
      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == SDL_BUTTON_LEFT) {
          P1Inputs->leftclick = 0;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          P1Inputs->rightclick = 0;
        }
        break;
    }
  }
}