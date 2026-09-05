#pragma once

#include <glm/glm.hpp>
#include <set>
#include <string>
#include <unordered_map>

#include "SDL_scancode.h"
#include "entity.h"
#include "map.h"
#include "model.h"
#include "particles.h"
#include "player.h"
#include "ui.h"

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

struct GlobalClass {
 public:
  std::string GameName = "CobblerGame";

  bool IsRunning;
  bool pause = false;

  struct Model {
    std::string texture;
    struct Vertex {
      glm::vec3 pos;
      std::string bone;
    };
    std::vector<Vertex> points;
    struct Face {
      std::array<uint32_t, 3> point;
      std::array<glm::vec2, 3> uv;
      glm::vec3 normal;
    };
    std::vector<Face> faces;
  };
  std::unordered_map<std::string, Model> Modelmap;

  std::vector<Modeltransform> Models;

  glm::mat4 perspectivematrix;

  bool LoggedIn = false;
  bool IsOnline = false;
  std::string playerclass = "default";

  char* pref_path;
};

struct ZipData {
  std::string startlevel, fontname;
  std::vector<std::string> stagenames;
};

struct Mapdata {
  struct Entitydata {
    std::string name;
    glm::vec3 pos;
  };
  std::vector<glm::vec3> HitboxPoints;
  std::vector<std::array<uint32_t, 3>> Hitboxmapfaces;
  std::vector<MapPoint> VisualPoints;
  std::vector<Mapface> Visualmapfaces;
  std::vector<glm::vec3> KillboxPoints;
  std::vector<std::array<uint32_t, 3>> KillboxFaces;
  std::vector<Entitydata> Entities;
  std::vector<Modeltransform> props;
  std::string skybox;
};
