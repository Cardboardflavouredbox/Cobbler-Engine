#version 330 core

uniform vec2 position;
uniform vec2 size;
uniform vec2 uvpos;
uniform vec2 uvsize;
uniform vec2 rotationdirection;

out vec2 uvout;

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
  result *= mat2(rotationdirection.x, rotationdirection.y, -rotationdirection.y,
                 rotationdirection.x);

  result += size / 2;

  result += position;
  uvresult.y = 1 - uvresult.y;
  uvout = uvresult;
  gl_Position = vec4(result, 0, 1);
}