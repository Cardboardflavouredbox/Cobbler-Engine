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
  std::vector<std::array<uint32_t, 3>> Faces;
  struct BSPtree {
    std::array<uint32_t, 3> Face;
    glm::vec3 normal;
    BSPtree* A;
    BSPtree* B;
  };
};