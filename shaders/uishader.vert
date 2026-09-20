#version 330 core

uniform vec2 position;
uniform vec2 size;
uniform vec2 uvpos;
uniform vec2 uvsize;

out vec2 uvout;

void main() {
  int ID = gl_VertexID;
  vec2 result = position;
  vec2 uvresult = uvpos;
  if (ID % 2 == 1) {
    result.x += size.x;
    uvresult += uvsize.x;
  }
  if (ID / 2 == 0) {
    result.y += size.y;
    uvresult += uvsize.y;
  }
  uvout = uvresult;
  gl_Position = vec4(result, 0, 1);
}