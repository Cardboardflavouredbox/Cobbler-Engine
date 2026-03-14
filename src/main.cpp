#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include <glaze/json.hpp>
#include <string>

#include "entity.h"
#include "global.h"
#include "input.h"
#include "render.h"
#include "update.h"
/* We will use this renderer to draw into this window every frame. */
GlobalClass* Global;
SettingsClass* Settings;
ZipData* LoadedData;
Entity* Camera;
Inputs* P1Inputs;
Uint32 lastTime;

bool init() {
  Global = static_cast<GlobalClass*>(calloc(1, sizeof(GlobalClass)));
  Settings = static_cast<SettingsClass*>(calloc(1, sizeof(SettingsClass)));
  P1Inputs = static_cast<Inputs*>(calloc(1, sizeof(Inputs)));
  Settings->resolutionx = 320;
  Settings->resolutiony = 200;
  Settings->fov = 90;

  ZipData tempzipdata;
  auto error = glz::read_file_json(tempzipdata, "MapStuff/resources.json",
                                   std::string{});
  if (error) {
    tempzipdata.texturenames.resize(2);
    tempzipdata.texturenames[0] = "Wall";
    tempzipdata.texturenames[1] = "Fence";
    tempzipdata.stagenames.resize(1);
    tempzipdata.stagenames[0] = "test";
    tempzipdata.startlevel = "test";
    error = glz::write_file_json<glz::opts{.prettify = true}>(
        tempzipdata, "MapStuff/resources.json", std::string{});
    if (error) return false;
  }
  LoadedData = &tempzipdata;

  Mapdata tempmapdata;
  error = glz::read_file_json(
      tempmapdata, "MapStuff/map/" + LoadedData->startlevel + ".json",
      std::string{});
  if (error) {
    tempmapdata.Points.resize(8);
    tempmapdata.Points[0] = Vector3({-1.5f, 1.5f, 2.f});
    tempmapdata.Points[1] = Vector3({1.5f, 1.5f, 2.f});
    tempmapdata.Points[2] = Vector3({1.5f, 1.5f, -1.f});
    tempmapdata.Points[3] = Vector3({-1.5f, 1.5f, -1.f});

    tempmapdata.Points[4] = Vector3({-1.5f, -1.5f, 2.f});
    tempmapdata.Points[5] = Vector3({1.5f, -1.5f, 2.f});
    tempmapdata.Points[6] = Vector3({1.5f, -1.5f, -1.f});
    tempmapdata.Points[7] = Vector3({-1.5f, -1.5f, -1.f});

    tempmapdata.mapfaces.resize(4);
    tempmapdata.mapfaces[0].points.resize(4);
    tempmapdata.mapfaces[0].points[0] = 0;
    tempmapdata.mapfaces[0].points[1] = 1;
    tempmapdata.mapfaces[0].points[2] = 2;
    tempmapdata.mapfaces[0].points[3] = 3;

    tempmapdata.mapfaces[1].points.resize(4);
    tempmapdata.mapfaces[1].points[0] = 4;
    tempmapdata.mapfaces[1].points[1] = 0;
    tempmapdata.mapfaces[1].points[2] = 3;
    tempmapdata.mapfaces[1].points[3] = 7;

    tempmapdata.mapfaces[2].points.resize(4);
    tempmapdata.mapfaces[2].points[0] = 5;
    tempmapdata.mapfaces[2].points[1] = 4;
    tempmapdata.mapfaces[2].points[2] = 7;
    tempmapdata.mapfaces[2].points[3] = 6;

    tempmapdata.mapfaces[3].points.resize(4);
    tempmapdata.mapfaces[3].points[0] = 1;
    tempmapdata.mapfaces[3].points[1] = 5;
    tempmapdata.mapfaces[3].points[2] = 6;
    tempmapdata.mapfaces[3].points[3] = 2;
    error = glz::write_file_json<glz::opts{.prettify = true}>(
        tempmapdata, "MapStuff/map/" + LoadedData->startlevel + ".json",
        std::string{});
    if (error) return false;
  }
  Global->Points = tempmapdata.Points;
  Global->mapfaces = tempmapdata.mapfaces;
  std::vector<SDL_Surface*> tempvector;
  Global->textures = tempvector;
  Global->textures.resize(32);

  Camera = static_cast<Entity*>(calloc(1, sizeof(Entity)));
  Camera->position = Vector3({0, 0, 0});

  if (!SDL_SetAppMetadata("BoomerShooter", "0.1", "com.example.myapp") ||
      !SDL_Init(SDL_INIT_VIDEO))
    return false;
  Global->window =
      SDL_CreateWindow("SDL3 window", Settings->resolutionx,
                       Settings->resolutiony, SDL_WINDOW_RESIZABLE);
  Global->renderer = SDL_CreateRenderer(Global->window, NULL);

  std::string basepath = SDL_GetBasePath(), tempstr = basepath;
  tempstr.append("/res/Color_palette.bmp");
  SDL_Surface* surface = SDL_LoadBMP(tempstr.c_str());
  surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_INDEX8);
  SDL_Color my_palette_colors[256];
  uint32_t* pixels = static_cast<uint32_t*>(surface->pixels);
  for (int i = 0; i < 256; i++) {
    SDL_Color tempcol;
    tempcol.a = 255;
    tempcol.r = (pixels[i] >> 16) & 0xFF;
    tempcol.g = (pixels[i] >> 8) & 0xFF;
    tempcol.b = pixels[i] & 0xFF;

    my_palette_colors[i] = tempcol;
  }
  SDL_DestroySurface(surface);

  SDL_Palette* palette = SDL_CreatePalette(256);
  SDL_SetPaletteColors(palette, my_palette_colors, 0, 256);

  Global->render_target = SDL_CreateSurface(
      Settings->resolutionx, Settings->resolutiony, SDL_PIXELFORMAT_INDEX8);
  SDL_SetSurfacePalette(Global->render_target, palette);

  for (int i = 0; i < LoadedData->texturenames.size(); i++) {
    tempstr = basepath;
    tempstr.append("/MapStuff/textures/" + LoadedData->texturenames[i] +
                   ".bmp");
    surface = SDL_LoadBMP(tempstr.c_str());
    surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    // SDL_SetSurfacePalette(surface, palette);
    Global->textures[i] = surface;
  }

  Global->IsRunning = true;
  SDL_SetRenderVSync(Global->renderer, 1);
  lastTime = SDL_GetTicks();
  SDL_SetRenderTarget(Global->renderer, NULL);
  return true;
}
void quit() {
  SDL_DestroyRenderer(Global->renderer);
  SDL_DestroyWindow(Global->window);
  SDL_DestroySurface(Global->render_target);
  // for (const auto& i : Global->texturemap) {
  //   SDL_DestroySurface(i.second);
  // }
  free(Global);
  free(Settings);
  free(Camera);
  free(P1Inputs);
  Global->IsRunning = false;
  SDL_Quit();
}

int main(int argc, char* argv[]) {
  if (!init()) {
    SDL_Log(SDL_GetError());
    return -1;
  }
  while (Global->IsRunning) {
    input();
    update();
    render();
  }
  quit();
  return 0;
}