#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include <string>

#include "entity.h"
#include "files.h"
#include "global.h"
#include "update.h"

GlobalClass* Global;
SettingsClass* Settings;
ZipData* LoadedData;
Entity* Camera;
Inputs* P1Inputs;
Uint32 lastTime;
Uint64 currentTime = SDL_GetPerformanceCounter();

bool editorinit() {
  Global->UImap.reserve(4);
  for (int i = 0; i < 2; i++) {
    std::deque<UIthing*> tempdeque;
    UIbox* box = new UIbox();
    box->color = 11;
    box->pos = Vector2({4, 4});
    box->size = Vector2({64, 128});

    tempdeque.push_back(box);

    UItext* text = new UItext();
    text->color = 0;
    text->pos = Vector2({8, 8});
    text->string = "test";

    TextandNumChanger* TaNC = new TextandNumChanger();
    if (i == 0) {
      TaNC->text = "Face";
      TaNC->num = &Global->editorselectedFace;
    } else if (i == 1) {
      TaNC->text = "Point";
      TaNC->num = &Global->editorselectedPoint;
    }

    text->TextChangerSet(TaNC);

    tempdeque.push_back(text);

    Global->UImap[i] = tempdeque;
  }
  return true;
}

int main(int argc, char* argv[]) {
  if (!init(false) || !editorinit()) {
    SDL_Log("%s", SDL_GetError());
    return -1;
  }
  SDL_Log("Init done");
  while (Global->IsRunning) {
    Uint64 start = SDL_GetPerformanceCounter();
    events();
    input();
    update();
    render();
    Uint64 result = (SDL_GetPerformanceCounter() - start);
    if (result < 1000000000 / (double)Settings->fps) {
      SDL_DelayNS(1000000000 / (double)Settings->fps - result);
    }
  }
  quit();
  return 0;
}