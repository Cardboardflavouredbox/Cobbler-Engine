#pragma once

#include <glm/vec3.hpp>
#include <string>

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

struct ModelGroupClass {
  std::vector<std::string> Models;
  struct Bone {
    glm::vec3 head, tail;
    std::string parent;
    struct Pose {
      glm::vec3 pos, scale, rot;
    };
    std::vector<Pose> Poses;
  };
  std::unordered_map<std::string, Bone> Bonemap;
};

LIB_API extern std::unordered_map<std::string, ModelGroupClass> ModelGroupMap;

struct Modeltransform {
  std::string name;
  glm::vec3 position, size = glm::vec3({1, 1, 1});
};
