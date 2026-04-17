#include "extern.h"
#include "map.h"
#include "update.h"

void rendergame() {}

void renderUI() {
  std::deque<UIthing*>* tempdeque = &Global->UImap[Global->UIindex];
  int len = tempdeque->size();
  for (int i = 0; i < len; i++) {
    tempdeque->at(i)->render();
  }
}

void render() {
  SDL_LockSurface(Global->render_target);

  Global->pixels = static_cast<unsigned char*>(Global->render_target->pixels);
  Global->pitch = Global->render_target->pitch;

  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      Global->pixelsdepth[i + j * Global->pitch] = 65535;
      Global->pixelstransparency[i + j * Global->pitch] = 255;
    }
  }
  renderUI();
  rendergame();

  unsigned char bgcolor = Global->rendermode == 1 ? 1 : 0;
  for (int i = 0; i < Settings->resolutionx; i++) {
    for (int j = 0; j < Settings->resolutiony; j++) {
      if (Global->pixelsdepth[i + j * Global->pitch] == 65535) {
        Global->pixels[i + j * Global->pitch] = bgcolor;
      }
    }
  }

  if (Global->pause || Global->isopeningfile) {
    for (int i = 0; i < Settings->resolutionx; i++) {
      for (int j = 0; j < Settings->resolutiony; j++) {
        Uint8 r, g, b;
        SDL_GetRGB(Global->pixels[i + j * Global->pitch],
                   SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_INDEX8),
                   Global->palette, &r, &g, &b);
        int r2 = r, g2 = g, b2 = b;
        r2 -= 64;
        g2 -= 64;
        b2 -= 64;
        if (r2 < 0) r2 = 0;
        if (g2 < 0) g2 = 0;
        if (b2 < 0) b2 = 0;
        Global->pixels[i + j * Global->pitch] =
            SDL_MapSurfaceRGB(Global->render_target, r2, g2, b2);
      }
    }
  }

  SDL_UnlockSurface(Global->render_target);

  // Screen size and position stuff
  int w = Global->windowx, h = Global->windowy, rtw = Global->render_target->w,
      rth = Global->render_target->h;
  int size = w / rtw;
  if (size > h / rth) size = h / rth;

  rtw *= size;
  rth *= size;

  w /= 2;
  h /= 2;
  w -= rtw / 2;
  h -= rth / 2;

  SDL_FRect temprect;
  temprect.w = rtw;
  temprect.h = rth;
  temprect.x = w;
  temprect.y = h;
  SDL_Texture* temptexture =
      SDL_CreateTextureFromSurface(Global->renderer, Global->render_target);
  SDL_SetTextureScaleMode(temptexture, SDL_SCALEMODE_PIXELART);
  SDL_RenderTexture(Global->renderer, temptexture, NULL, &temprect);
  SDL_RenderPresent(Global->renderer);
}