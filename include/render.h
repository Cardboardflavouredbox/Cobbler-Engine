#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#ifdef _WIN32
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT
#endif

LIB_EXPORT void DrawLine(unsigned char color, glm::vec3 rawvectors[]);

LIB_EXPORT void DrawCircle(unsigned char color, glm::vec3 rawpoint, int radius);

LIB_EXPORT void DrawTri(std::string texture, glm::vec3 rawvectors[],
                        glm::vec2 UVs[], int xloop, int yloop);
