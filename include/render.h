#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>

#include "model.h"

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

LIB_API void renderModelGroup(Modeltransform modeltrans,
                              ModelGroupClass* modelgroup);

LIB_API void DrawLine(unsigned char color, glm::vec3 rawvectors[]);

LIB_API void DrawCircle(unsigned char color, glm::vec3 rawpoint, int radius);

LIB_API void DrawTri(std::string texture, glm::vec3 rawvectors[],
                     glm::vec2 UVs[], int xloop, int yloop);
