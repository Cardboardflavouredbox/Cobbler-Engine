#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in uint boneindex;

uniform mat4 model;

uniform uint bonelist[64];
uniform mat4 restmat[64];
uniform vec3 bonehead[64];
uniform mat4 resultbonemat[64];
uniform mat4 transformmat;

uniform int hasaction;
uniform float lookdirx;

out vec4 vertexColor;
out vec2 TexCoord;

int getboneindexreal() {
  for (int i = 0; i < 64; i++) {
    if (bonelist[i] == boneindex)
      return i;
  }
  return 0;
}

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
  int index = getboneindexreal();
  vec4 result = vec4(aPos, 1.0);
  if (hasaction > 0) {
    result = restmat[index] * result;

    result -= vec4(bonehead[index], 0.0);

    result = resultbonemat[index] * result;
  }

  result =
      vec4(qtransform(angleAxis(lookdirx * 3.141592741 / 180.0, vec3(0, 0, 1)),
                      result.xyz),
           1.0);

  result = transformmat * result;

  gl_Position = model * result;
  TexCoord = aTexCoord;
}