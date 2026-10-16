#pragma once

#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

#include "map.h"
#include "model.h"

struct Mapdata {
  struct Entitydata {
    std::string name;
    glm::vec3 pos;
  };
  std::vector<glm::vec3> HitboxPoints;
  std::vector<std::array<uint32_t, 3>> Hitboxmapfaces;
  std::vector<VisualObject> VisualObjectsVector;
  std::vector<glm::vec3> KillboxPoints;
  std::vector<std::array<uint32_t, 3>> KillboxFaces;
  std::vector<Entitydata> Entities;
  std::vector<Modeltransform> props;
  std::string skybox;
  std::vector<glm::vec3> SpawnPoints;
};

struct MapBSP {
  std::vector<glm::vec3> Points;
  struct BSPnode {
    std::vector<std::array<uint32_t, 3>> Faces;
    glm::vec4 plane;
    int A = -1, B = -1;
  };
  std::vector<BSPnode> BSPtree;
};

LIB_API extern MapBSP* GlobalMapBSP;

extern "C" {
LIB_API void recursiveBSPadd(int index,
                             std::vector<std::array<uint32_t, 3>>& out);
}