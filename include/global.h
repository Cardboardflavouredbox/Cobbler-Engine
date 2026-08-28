#pragma once

#include <SDL3/SDL_scancode.h>

#include <glm/glm.hpp>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>

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

const double PI =
    3.1415926535897932384626433832795028841971693993751058209749445923078164062;

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
      std::array<unsigned int, 3> point;
      std::array<glm::vec2, 3> uv;
      glm::vec3 normal;
    };
    std::vector<Face> faces;
  };
  std::unordered_map<std::string, Model> Modelmap;

  std::map<unsigned int, Entity*> Entities;
  std::map<unsigned int, Particle*> Particles;
  std::vector<Modeltransform> Models;

  std::queue<unsigned int> EntitydeleteQueue;
  std::queue<unsigned int> ParticledeleteQueue;

  glm::mat4 perspectivematrix;

  bool LoggedIn = false;
  bool IsOnline = false;
  std::string playerclass = "default";

  char* pref_path;
};

struct GlobalNetworkClass {
  bool IsServer;
  uint64_t UserID;
  std::set<uint64_t> UserIDs;
  std::unordered_map<uint64_t, float> PlayerTimecounter;
  std::unordered_map<uint64_t, Entity*> PlayerEntity;
  std::unordered_map<uint64_t, playerinputs> PlayerInputList;

  float Onlinesendwait = 0.05f;
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
  std::vector<MapPoint> Points;
  std::vector<Mapface> mapfaces;
  std::vector<glm::vec3> KillboxPoints;
  std::vector<std::vector<int>> KillboxFaces;
  std::vector<Entitydata> Entities;
  std::vector<Modeltransform> props;
  std::string skybox;
};
