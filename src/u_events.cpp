#include <glad/glad.h>

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
      case SDL_EVENT_WINDOW_RESIZED:
        SDL_GetWindowSizeInPixels(Global->window, &Global->windowx,
                                  &Global->windowy);
        switch (Settings->graphicsmode) {
          case 0: {
            Global->SRstuff->pixelsdepth.resize(Settings->resolutionx *
                                                Settings->resolutiony);
            Global->SRstuff->pixelstransparency.resize(Settings->resolutionx *
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
      case SDL_EVENT_MOUSE_WHEEL:
        P1Inputs->MouseScroll.x = event.wheel.x;
        P1Inputs->MouseScroll.y = event.wheel.y;
        break;
    }
  }
}