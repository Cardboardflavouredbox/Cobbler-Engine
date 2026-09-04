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

struct MapPoint {
  glm::vec3 pos;
  std::array<float, 3> shade = {255, 255, 255};
};

struct Mapface {
  bool doublesided = false;
  std::string texture;
  std::vector<int> points;
  std::vector<glm::vec2> UVs;
};

struct GlobalMapClass {
  std::vector<MapPoint> Points;
  std::vector<Mapface> mapfaces;
  std::vector<glm::vec3> KillboxPoints;
  std::vector<std::vector<int>> KillboxFaces;
  std::string skybox;
};

LIB_API extern std::unique_ptr<GlobalMapClass> GlobalMapStuff;