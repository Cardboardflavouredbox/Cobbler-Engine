#include <cmath>

#include "rendermath.h"

float Areathing(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
  return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

glm::vec3 GetUV(glm::vec2 P, glm::vec2 R1, glm::vec2 R2, glm::vec2 R3) {
  float det = Areathing(R1, R2, R3);
  glm::vec3 a = glm::vec3(
      {Areathing(R2, R3, P), Areathing(R3, R1, P), Areathing(R1, R2, P)});
  a /= det;
  return a;
}

float anglething(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
  glm::vec2 ab = b - a;
  glm::vec2 cb = b - c;

  float dot = (ab.x * cb.x + ab.y * cb.y);
  float cross = (ab.x * cb.y - ab.y * cb.x);

  float alpha = std::atan2(cross, dot);

  return alpha * 180.f / PI + 0.5f;
}

bool Vec2inTri(glm::vec2 p, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3) {
  float s1 = anglething(v3, p, v1), s2 = anglething(v1, p, v2),
        s3 = anglething(v2, p, v3);

  return (s1 + s2 + s3 > 360);
}
