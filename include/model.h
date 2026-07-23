#pragma once

#include <array>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <map>
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

struct ModelGroupClass {
  std::vector<std::string> Models;
  struct Bone {
    glm::vec3 head, tail;
    std::string parent;
    struct Pose {
      glm::vec3 pos, scale;
      glm::quat rot;
    };
    std::unordered_map<std::string, std::map<unsigned int, Pose>> Poses;
  };
  std::unordered_map<std::string, Bone> Bonemap;
  std::unordered_map<std::string, std::array<unsigned int, 2>> anim;
};

LIB_API extern std::unordered_map<std::string, ModelGroupClass> ModelGroupMap;

struct Modeltransform {
  std::string name;
  struct action {
    std::string name;
    float frame;
  };
  std::vector<action> actions;
  glm::vec3 position, size = glm::vec3({1, 1, 1});
  glm::vec2 lookdir;
  glm::quat rot;
};
