#version 330 core

uniform vec2 position;
uniform vec2 size;
uniform vec2 uvpos;
uniform vec2 uvsize;
uniform float rotateangle;
uniform ivec2 screensize;

out vec2 uvout;

vec2 rotate(vec2 v, float a) {
  float s = sin(a);
  float c = cos(a);
  mat2 m = mat2(c, s, -s, c);
  return m * v;
}

void main() {
  int ID = gl_VertexID;
  vec2 result = vec2(0, 0);
  vec2 uvresult = uvpos;

  if (ID % 2 == 1) {
    result.x += size.x / 2;
    uvresult.x += uvsize.x;
  } else {
    result.x -= size.x / 2;
  }
  if (ID / 2 == 0) {
    result.y += size.y / 2;
    uvresult.y += uvsize.y;
  } else {
    result.y -= size.y / 2;
  }
  result = rotate(result, rotateangle);

  result += size / 2;

  vec2 realpos = position;
  vec2 realsize = size;
  realpos.x *= 2 / float(screensize.x);
  realpos.y *= 2 / float(screensize.y);
  realpos.x -= 1;
  realpos.y -= 1;

  realpos.y *= -1;

  realsize.x *= 2 / float(screensize.x);
  realsize.y *= 2 / float(screensize.y);

  realpos.y -= realsize.y;

  result.x *= 2 / float(screensize.x);
  result.t *= 2 / float(screensize.y);

  result += realpos;
  uvresult.y = 1 - uvresult.y;
  uvout = uvresult;
  gl_Position = vec4(result, 0, 1);
}