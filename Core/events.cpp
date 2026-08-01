#include <glad/glad.h>

#include "extern.h"
#include "update.h"

// process events
void events() {
  // If mouse already clicked(2) change value to 1.
  // This allows the game to tell if you've clicked this frame or not.
  if (LocalInputs->leftclick == 2) LocalInputs->leftclick = 1;
  if (LocalInputs->rightclick == 2) LocalInputs->rightclick = 1;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      // Quit game
      case SDL_EVENT_QUIT:
        Global->IsRunning = false;
        break;
      // Resize Window
      case SDL_EVENT_WINDOW_RESIZED:
        // Get Window Size
        SDL_GetWindowSizeInPixels(Global->window, &Global->windowx,
                                  &Global->windowy);

        // Get Window Scale
        Global->windowscale = SDL_GetWindowDisplayScale(Global->window);

        switch (Settings->graphicsmode) {
          case 0: {  // software
            // resize pixelsdepth buffer
            Global->SRstuff->pixelsdepth.resize(Settings->resolutionx *
                                                Settings->resolutiony);
            break;
          }
          case 1: {  // opengl
            // Set glViewport to middle of window.
            int w = Global->windowx, h = Global->windowy,
                rtw = Settings->resolutionx, rth = Settings->resolutiony;
            int size = w / rtw;
            if (size > h / rth) size = h / rth;

            rtw *= size;
            rth *= size;

            w /= 2;
            h /= 2;
            w -= rtw / 2;
            h -= rth / 2;

            glViewport(w, h, rtw, rth);
            break;
          }
        }
        break;
      // Mouse Click read
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          LocalInputs->leftclick = 2;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          LocalInputs->rightclick = 2;
        }
        break;
      // Mouse Unclick read
      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == SDL_BUTTON_LEFT) {
          LocalInputs->leftclick = 0;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          LocalInputs->rightclick = 0;
        }
        break;
      // Mouse Wheel read
      case SDL_EVENT_MOUSE_WHEEL:
        LocalInputs->MouseScroll.x = event.wheel.x;
        LocalInputs->MouseScroll.y = event.wheel.y;
        break;
    }
  }
}