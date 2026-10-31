#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_log.h>
#include <glad/gl.h>

#include "SDL_scancode.h"
#include "commandline.h"
#include "deltaTime.h"
#include "extern.h"
#include "inputs.h"
#include "render.h"
#include "settings.h"
#include "update.h"

bool textinputing = false;
float backspacetime = 0;

// process events
void events() {
  if (textinputstate > 0) {
    textinputstate = 0;
    SDL_StartTextInput(RendererGlobal->window);
    textinputing = true;
  } else if (textinputstate < 0) {
    textinputstate = 0;
    SDL_StopTextInput(RendererGlobal->window);
    textinputing = false;
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      // Text input
      case SDL_EVENT_TEXT_INPUT: {
        if (textinputing) {
          cmdcurrentinput.append(event.text.text);
        }
        break;
      }
      // Quit game
      case SDL_EVENT_QUIT: {
        IsRunning = false;
        break;
      }
      // Resize Window
      case SDL_EVENT_WINDOW_RESIZED: {
        // Get Window Size
        SDL_GetWindowSizeInPixels(RendererGlobal->window,
                                  &RendererGlobal->windowx,
                                  &RendererGlobal->windowy);

        // Get Window Scale
        RendererGlobal->windowscale =
            SDL_GetWindowDisplayScale(RendererGlobal->window);

        switch (Settings->graphicsmode) {
          case Software: {  // software
            // resize pixelsdepth buffer
            RendererGlobal->SRstuff->pixelsdepth.resize(Settings->resolutionx *
                                                        Settings->resolutiony);
            break;
          }
          case OpenGL4:
          case OpenGL3: {  // opengl newer
            float w = RendererGlobal->windowx, h = RendererGlobal->windowy,
                  rtw = Settings->resolutionx, rth = Settings->resolutiony;
            float size = w / rtw;
            if (size > h / rth) size = h / rth;

            rtw *= size;
            rth *= size;

            w /= 2.f;
            h /= 2.f;
            w -= rtw / 2.f;
            h -= rth / 2.f;

            RendererGlobal->viewportdata[0] = std::roundf(w);
            RendererGlobal->viewportdata[1] = std::roundf(h);
            RendererGlobal->viewportdata[2] = std::roundf(rtw);
            RendererGlobal->viewportdata[3] = std::roundf(rth);
            break;
          }
          case OpenGL1: {  // opengl 1
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
      }
      // Mouse Click read
      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        if (event.button.button == SDL_BUTTON_LEFT) {
          LocalInputs->leftclick = 2;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          LocalInputs->rightclick = 2;
        }
        break;
      }
      // Mouse Unclick read
      case SDL_EVENT_MOUSE_BUTTON_UP: {
        if (event.button.button == SDL_BUTTON_LEFT) {
          LocalInputs->leftclick = 0;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          LocalInputs->rightclick = 0;
        }
        break;
      }
      // Mouse Wheel read
      case SDL_EVENT_MOUSE_WHEEL: {
        LocalInputs->MouseScroll.x = event.wheel.x;
        LocalInputs->MouseScroll.y = event.wheel.y;
        break;
      }
      case SDL_EVENT_KEY_DOWN: {
        if (textinputing && event.key.scancode != SDL_SCANCODE_BACKSPACE &&
            event.key.scancode != SDL_SCANCODE_RETURN) {
          LocalInputs->Keys[event.key.scancode] = 0;
        } else {
          LocalInputs->Keys[event.key.scancode] = event.key.repeat ? 1 : 2;
        }
        break;
      }
      case SDL_EVENT_KEY_UP: {
        LocalInputs->Keys[event.key.scancode] = 0;
        break;
      }
    }
  }
  if (textinputing) {
    if (LocalInputs->Keys[SDL_SCANCODE_BACKSPACE] > 0 &&
        !cmdcurrentinput.empty()) {
      if (LocalInputs->Keys[SDL_SCANCODE_BACKSPACE] == 2) {
        cmdcurrentinput.pop_back();
        backspacetime = 0.5f;
      } else if (backspacetime < 0.f) {
        cmdcurrentinput.pop_back();
        backspacetime = 1 / 64.f;
      }
      backspacetime -= renderdeltaTime;
    }
    if (LocalInputs->Keys[SDL_SCANCODE_RETURN] > 1) {
      pushcmd();
      LocalInputs->Keys[SDL_SCANCODE_RETURN] = 1;
    }
  }
}