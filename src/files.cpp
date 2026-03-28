#include "files.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_dialog.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <glaze/json.hpp>

#include "extern.h"
#include "global.h"

static const SDL_DialogFileFilter filters[] = {{"JSON file", "json"},
                                               {"All files", "*"}};

static void SDLCALL callback(void* userdata, const char* const* filelist,
                             int filter) {
  Global->isopeningfile = false;
  if (!filelist) {
    SDL_Log("An error occured: %s", SDL_GetError());
    return;
  } else if (!*filelist) {
    SDL_Log("The user did not select any file.");
    SDL_Log("Most likely, the dialog was canceled.");
    return;
  }

  while (*filelist) {
    SDL_Log("Full path to selected file: '%s'", *filelist);
    Mapdata tempmapdata;
    tempmapdata.skybox = Global->skybox;
    tempmapdata.Points = Global->Points;
    tempmapdata.mapfaces = Global->mapfaces;
    auto error = glz::write_file_json<glz::opts{.prettify = true}>(
        tempmapdata, *filelist, std::string{});
    if (error) {
      return;
    }
    filelist++;
  }

  if (filter < 0) {
    SDL_Log(
        "The current platform does not support fetching "
        "the selected filter, or the user did not select"
        " any filter.");
    return;
  } else if (filter < SDL_arraysize(filters)) {
    SDL_Log("The filter selected by the user is '%s' (%s).",
            filters[filter].pattern, filters[filter].name);
    return;
  }
}

void savemap() {
  Global->isopeningfile = true;
  SDL_ShowSaveFileDialog(callback, NULL, Global->window, filters,
                         SDL_arraysize(filters), NULL);
}

CustomGlyphthing CreateGlyph(FT_GlyphSlot glyph) {
  CustomGlyphthing temp;
  temp.width = glyph->bitmap.width;
  temp.height = glyph->bitmap.rows;
  temp.pitch = glyph->bitmap.pitch;
  temp.advancex = glyph->advance.x / 64;
  temp.advancey = glyph->advance.y / 64;
  temp.offsetx = glyph->bitmap_left;
  temp.offsety = glyph->bitmap_top;

  temp.pixels = new unsigned char[temp.width * temp.height];
  for (int i = 0; i < temp.width * temp.height; i++) {
    temp.pixels[i] = glyph->bitmap.buffer[i];
  }
  return temp;
}

bool init(bool hidemouse) {
  Global = new GlobalClass();
  Settings = new SettingsClass();
  P1Inputs = new Inputs();
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
    tempmapdata.Points[0] = Vector3({-15.f, 15.f, 2.f});
    tempmapdata.Points[1] = Vector3({15.f, 15.f, 2.f});
    tempmapdata.Points[2] = Vector3({15.f, 15.f, -1.f});
    tempmapdata.Points[3] = Vector3({-15.f, 15.f, -1.f});

    tempmapdata.Points[4] = Vector3({-15.f, -15.f, 2.f});
    tempmapdata.Points[5] = Vector3({15.f, -15.f, 2.f});
    tempmapdata.Points[6] = Vector3({15.f, -15.f, -1.f});
    tempmapdata.Points[7] = Vector3({-15.f, -15.f, -1.f});

    tempmapdata.mapfaces.resize(5);
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

    tempmapdata.mapfaces[4].points.resize(4);
    tempmapdata.mapfaces[4].points[0] = 3;
    tempmapdata.mapfaces[4].points[1] = 2;
    tempmapdata.mapfaces[4].points[2] = 6;
    tempmapdata.mapfaces[4].points[3] = 7;
    tempmapdata.mapfaces[4].xloop = 10;
    tempmapdata.mapfaces[4].yloop = 10;
    tempmapdata.mapfaces[4].texture = 2;

    for (int i = 0; i < 4; i++) {
      tempmapdata.mapfaces[i].xloop = 8;
    }
    for (int i = 0; i < 5; i++) {
      tempmapdata.mapfaces[i].UVs = {Vector2({0, 0}), Vector2({1, 0}),
                                     Vector2({1, 1}), Vector2({0, 1})};
    }
    error = glz::write_file_json<glz::opts{.prettify = true}>(
        tempmapdata, "MapStuff/map/" + LoadedData->startlevel + ".json",
        std::string{});
    if (error) return false;
  }
  Global->Points = tempmapdata.Points;
  Global->mapfaces = tempmapdata.mapfaces;
  Global->skybox = tempmapdata.skybox;
  std::vector<SDL_Surface*> tempvector;
  tempvector.resize(32);
  Global->textures = tempvector;

  for (int i = 0; i < Global->mapfaces.size(); i++) {
    if (Global->mapfaces[i].points.size() == 4) {
      Mapface temp;
      temp.doublesided = Global->mapfaces[i].doublesided;
      temp.xloop = Global->mapfaces[i].xloop;
      temp.yloop = Global->mapfaces[i].yloop;
      temp.texture = Global->mapfaces[i].texture;
      int temppoints[3] = {Global->mapfaces[i].points[0],
                           Global->mapfaces[i].points[1],
                           Global->mapfaces[i].points[2]};
      Vector2 tempUV[3] = {Global->mapfaces[i].UVs[0],
                           Global->mapfaces[i].UVs[1],
                           Global->mapfaces[i].UVs[2]};
      temp.points.assign(temppoints, temppoints + 3);
      temp.UVs.assign(tempUV, tempUV + 3);
      Global->mapfaces.push_back(temp);

      temp.points[0] = Global->mapfaces[i].points[2];
      temp.points[1] = Global->mapfaces[i].points[3];
      temp.points[2] = Global->mapfaces[i].points[0];

      temp.UVs[0] = Global->mapfaces[i].UVs[2];
      temp.UVs[1] = Global->mapfaces[i].UVs[3];
      temp.UVs[2] = Global->mapfaces[i].UVs[0];

      Global->mapfaces.push_back(temp);

      Global->mapfaces.erase(Global->mapfaces.begin() + i);
      i--;
    }
  }

  Camera = new Entity();
  Camera->position = Vector3({0, 0, 3});

  Global->Entities.push_back(Camera);

  if (!SDL_SetAppMetadata("CobblerEngine", "0.1", "com.example.myapp") ||
      !SDL_Init(SDL_INIT_VIDEO))
    return false;
  Global->window =
      SDL_CreateWindow("Cobbler Engine", Settings->resolutionx,
                       Settings->resolutiony, SDL_WINDOW_RESIZABLE);
  Global->renderer = SDL_CreateRenderer(Global->window, NULL);

  SDL_SetWindowRelativeMouseMode(Global->window, hidemouse);

  std::string basepath = SDL_GetBasePath(), tempstr = basepath;
  tempstr.append("/res/Color_palette.bmp");
  SDL_Surface* surface = SDL_LoadBMP(tempstr.c_str());

  Global->palette = SDL_GetSurfacePalette(surface);

  Global->render_target = SDL_CreateSurface(
      Settings->resolutionx, Settings->resolutiony, SDL_PIXELFORMAT_INDEX8);
  SDL_SetSurfacePalette(Global->render_target, Global->palette);

  for (int i = 0; i < LoadedData->texturenames.size(); i++) {
    tempstr = basepath;
    tempstr.append("/MapStuff/textures/" + LoadedData->texturenames[i] +
                   ".bmp");
    surface = SDL_LoadBMP(tempstr.c_str());
    surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    // SDL_SetSurfacePalette(surface, palette);
    Global->textures[i] = surface;
  }

  if (FT_Init_FreeType(&(Global->FTlibrary))) return false;

  tempstr = basepath;
  tempstr.append("/res/Galmuri11.bdf");
  if (FT_New_Face(Global->FTlibrary, tempstr.c_str(), 0, &(Global->FTface)))
    return false;

  FT_Set_Pixel_Sizes(Global->FTface, 0, 12);

  for (int i = 0; i < 128; i++) {
    FT_UInt glyph_index = FT_Get_Char_Index(Global->FTface, i);
    FT_Load_Glyph(Global->FTface, glyph_index, FT_LOAD_MONOCHROME);
    FT_Render_Glyph(Global->FTface->glyph, FT_RENDER_MODE_MONO);

    Global->Glyphmap[glyph_index] = CreateGlyph(Global->FTface->glyph);
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
  for (const auto& i : Global->textures) {
    SDL_DestroySurface(i);
  }
  delete (Global);
  delete (Settings);
  delete (Camera);
  delete (P1Inputs);

  FT_Done_Face(Global->FTface);
  FT_Done_FreeType(Global->FTlibrary);

  Global->IsRunning = false;
  SDL_Quit();
}
