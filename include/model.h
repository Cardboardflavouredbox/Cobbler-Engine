#pragma once

#include <array>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <map>
#include <string>
#include <unordered_map>
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
    uint32_t parent;
    struct Pose {
      glm::vec3 pos, scale;
      glm::quat rot;
    };
    Pose restpose;
    std::unordered_map<uint32_t, std::map<uint32_t, Pose>> Poses;
  };
  struct visibilitything {
    std::string name;
    std::map<uint32_t, bool> value;
  };
  std::unordered_map<uint32_t, std::vector<visibilitything>> modelvisibility;

  std::unordered_map<uint32_t, Bone> Bonemap;
  std::unordered_map<uint32_t, std::array<uint32_t, 2>> anim;
};

LIB_API extern std::unordered_map<std::string, uint32_t> BonetoInt;
LIB_API extern uint32_t newboneindex;

LIB_API extern std::unordered_map<std::string, uint32_t> PosetoInt;
LIB_API extern uint32_t newposeindex;

LIB_API extern std::unordered_map<std::string, ModelGroupClass> ModelGroupMap;

struct Modeltransform {
  bool visible = true;
  std::string name;
  struct action {
    std::string name;
    float frame, speed = 1.f;
  };
  std::vector<action> actions;
  glm::vec3 position, size = glm::vec3({1, 1, 1});
  glm::vec2 lookdir;
  glm::quat rot;

  struct BoneResult {
    glm::vec3 head, scale;
    glm::quat rot;
  };

  std::vector<uint32_t> Bonecodevec;
  std::unordered_map<uint32_t, BoneResult> Bonemap;
  std::unordered_map<std::string, bool> modelvisibilityresult;
};
