#include <glad/glad.h>

#include "extern.h"
#include "inputs.h"
#include "render.h"
#include "settings.h"
#include "update.h"

// process events
void events() {
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
        SDL_GetWindowSizeInPixels(RendererGlobal->window,
                                  &RendererGlobal->windowx,
                                  &RendererGlobal->windowy);

        // Get Window Scale
        RendererGlobal->windowscale =
            SDL_GetWindowDisplayScale(RendererGlobal->window);

        switch (Settings->graphicsmode) {
          case 0: {  // software
            // resize pixelsdepth buffer
            RendererGlobal->SRstuff->pixelsdepth.resize(Settings->resolutionx *
                                                        Settings->resolutiony);
            break;
          }
          case 1: {  // opengl
            // Set glViewport to middle of window.
            int w = RendererGlobal->windowx, h = RendererGlobal->windowy,
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
      case SDL_EVENT_KEY_DOWN:
        LocalInputs->Keys[event.key.scancode] = event.key.repeat ? 1 : 2;
        break;
      case SDL_EVENT_KEY_UP:
        LocalInputs->Keys[event.key.scancode] = 0;
        break;
    }
  }
}