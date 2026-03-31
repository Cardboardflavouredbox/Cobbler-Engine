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

    UItext* text[4] = {new UItext(), new UItext(), new UItext(), new UItext()};
    text[0]->color = 0;
    text[0]->pos = Vector2({8, 8});
    text[0]->string = "test";

    TextandNumChanger* TaNC = new TextandNumChanger();
    if (i == 0) {
      TaNC->text = "Face";
      TaNC->num = &Global->editorselectedFace;
      TextandGlobalFacePointChanger* TaGFPC[3] = {
          new TextandGlobalFacePointChanger(),
          new TextandGlobalFacePointChanger(),
          new TextandGlobalFacePointChanger()};
      for (int j = 1; j < 4; j++) {
        text[j]->color = 0;
        text[j]->pos = Vector2({8, 8 + 12.f * j});
        text[j]->string = "test";
        TaGFPC[j - 1]->text = "Point";
        TaGFPC[j - 1]->num = &Global->editorselectedFace;
        TaGFPC[j - 1]->pointindex = j - 1;
        text[j]->TextChangerSet(TaGFPC[j - 1]);
      }
    } else if (i == 1) {
      TaNC->text = "Point";
      TaNC->num = &Global->editorselectedPoint;
      TextandGlobalPointChanger* TaGPC[3] = {new TextandGlobalPointChanger(),
                                             new TextandGlobalPointChanger(),
                                             new TextandGlobalPointChanger()};

      TaGPC[0]->text = "X ";
      TaGPC[0]->num = &Global->editorselectedPoint;
      TaGPC[0]->xyz = 0;
      TaGPC[1]->text = "Y ";
      TaGPC[1]->num = &Global->editorselectedPoint;
      TaGPC[1]->xyz = 1;
      TaGPC[2]->text = "Z ";
      TaGPC[2]->num = &Global->editorselectedPoint;
      TaGPC[2]->xyz = 2;

      for (int j = 1; j < 4; j++) {
        text[j]->color = 0;
        text[j]->pos = Vector2({8, 8 + 12.f * j});
        text[j]->string = "test";
        text[j]->TextChangerSet(TaGPC[j - 1]);
      }
    }
    text[0]->TextChangerSet(TaNC);

    for (int j = 0; j < 4; j++) {
      tempdeque.push_back(text[j]);
    }

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