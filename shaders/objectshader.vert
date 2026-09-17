#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform vec3 bonehead;

uniform mat4 restmat;

uniform mat4 resultbonemat;

uniform mat4 model;

uniform int hasaction;
uniform float lookdirx;
uniform mat4 transformmat;

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

vec4 angleAxis(float angle, vec3 v) {
  float a = angle;
  float s = sin(a * 0.5);

  return vec4(v * s, cos(a * 0.5));
}

void main() {

  vec4 result = vec4(aPos, 1.0);
  if (hasaction > 0) {
    result = restmat * result;

    result -= vec4(bonehead, 0.0);

    result = resultbonemat * result;
  }

  result =
      vec4(qtransform(angleAxis(lookdirx * 3.141592741 / 180.0, vec3(0, 0, 1)),
                      result.xyz),
           1.0);

  result = transformmat * result;

  gl_Position = model * result;
  TexCoord = aTexCoord;
}