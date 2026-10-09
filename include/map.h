#pragma once
#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

struct Mapface {
  bool doublesided = false;
  std::array<uint32_t, 3> points;
  struct TUVthing {
    std::string texture;
    std::array<glm::vec2, 3> UVs;
  };
  std::vector<TUVthing> TUVvector;
};

struct VisualObject {
  std::vector<glm::vec3> VisualPoints;
  std::vector<Mapface> Visualmapfaces;
};

struct GlobalMapClass {
  std::vector<glm::vec3> HitboxPoints;
  std::vector<std::array<uint32_t, 3>> Hitboxmapfaces;

  std::vector<glm::vec3> KillboxPoints;
  std::vector<std::array<uint32_t, 3>> KillboxFaces;

  std::vector<VisualObject> VisualObjectsVector;
  std::string skybox;
  std::vector<glm::vec3> SpawnPoints;
};

LIB_API extern std::unique_ptr<GlobalMapClass> GlobalMapStuff;

extern "C" {
LIB_API glm::vec3 GetRespawnPoint(int teamid);
}
