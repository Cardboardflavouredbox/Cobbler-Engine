#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform vec3 bonehead;

uniform vec3 restpos;
uniform vec3 restscale;
uniform vec4 restrot;

uniform vec3 resultbonehead;
uniform vec3 resultbonescale;
uniform vec4 resultbonerot;

uniform mat4 model;

uniform int hasaction;
uniform float lookdirx;
uniform vec4 rot;
uniform vec3 size;
uniform vec3 position;

out vec4 vertexColor;
out vec2 TexCoord;

vec3 qtransform(vec4 q, vec3 v) {

  float tx = 2.0 * (q.y * v.z - q.z * v.y);
  float ty = 2.0 * (q.z * v.x - q.x * v.z);
  float tz = 2.0 * (q.x * v.y - q.y * v.x);

  // v + w t + cross(q.xyz, t)
  float vx = v.x + q.w * tx + q.y * tz - q.z * ty;
  float vy = v.y + q.w * ty + q.z * tx - q.x * tz;
  float vz = v.z + q.w * tz + q.x * ty - q.y * tx;

  return vec3(vx, vy, vz);
}

// https://stackoverflow.com/a/19740748
mat4 lookAt(vec3 dir, vec3 up) {
  vec3 f = dir;
  vec3 u = normalize(up);
  vec3 s = normalize(cross(f, u));
  u = cross(s, f);

  mat4 Result = mat4(1.0);
  Result[0][0] = s.x;
  Result[1][0] = s.y;
  Result[2][0] = s.z;
  Result[0][1] = u.x;
  Result[1][1] = u.y;
  Result[2][1] = u.z;
  Result[0][2] = -f.x;
  Result[1][2] = -f.y;
  Result[2][2] = -f.z;
  Result[3][0] = -dot(s, vec3(0));
  Result[3][1] = -dot(u, vec3(0));
  Result[3][2] = dot(f, vec3(0));
  return Result;
}

// I got these three functions from the glm headers.
float getangle(vec4 quatinput) {
  if (abs(quatinput.w) > 0.87758255) {
    float a = asin(sqrt(quatinput.x * quatinput.x + quatinput.y * quatinput.y +
                        quatinput.z * quatinput.z)) *
              2;
    if (quatinput.w < 0)
      return 3.141592741 * 2 - a;
    return a;
  }

  return acos(quatinput.w) * 2;
}

vec3 getaxis(vec4 quatinput) {
  float tmp1 = 1 - quatinput.w * quatinput.w;
  if (tmp1 <= 0)
    return vec3(0, 0, 1);
  float tmp2 = 1 / sqrt(tmp1);
  return vec3(quatinput.x * tmp2, quatinput.y * tmp2, quatinput.z * tmp2);
}

vec4 angleAxis(float angle, vec3 v) {
  float a = angle;
  float s = sin(a * 0.5);

  return vec4(v * s, cos(a * 0.5));
}

void main() {

  vec3 result = aPos;
  if (hasaction > 0) {
    result *= restscale;
    result = qtransform(restrot, result);
    result += restpos;

    result -= bonehead;

    result = resultbonescale * qtransform(resultbonerot, result);

    result += resultbonehead;
  }

  result = qtransform(angleAxis(lookdirx * 3.141592741 / 180.0, vec3(0, 0, 1)),
                      result);

  gl_Position = model * vec4(position + qtransform(rot, result * size), 1.0);
  TexCoord = aTexCoord;
}