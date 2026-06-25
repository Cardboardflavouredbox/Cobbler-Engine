#include <glad/glad.h>

#include "extern.h"
#include "update.h"
void events() {
  if (LocalInputs->leftclick == 2) LocalInputs->leftclick = 1;
  if (LocalInputs->rightclick == 2) LocalInputs->rightclick = 1;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        Global->IsRunning = false;
        break;
      case SDL_EVENT_WINDOW_RESIZED:
        SDL_GetWindowSizeInPixels(Global->window, &Global->windowx,
                                  &Global->windowy);
        switch (Settings->graphicsmode) {
          case 0: {
            Global->SRstuff->pixelsdepth.resize(Settings->resolutionx *
                                                Settings->resolutiony);
            break;
          }
          case 1: {
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
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          LocalInputs->leftclick = 2;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          LocalInputs->rightclick = 2;
        }
        break;
      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == SDL_BUTTON_LEFT) {
          LocalInputs->leftclick = 0;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          LocalInputs->rightclick = 0;
        }
        break;
      case SDL_EVENT_MOUSE_WHEEL:
        LocalInputs->MouseScroll.x = event.wheel.x;
        LocalInputs->MouseScroll.y = event.wheel.y;
        break;
    }
  }
}