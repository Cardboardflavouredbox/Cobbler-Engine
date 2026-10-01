#version 330 core

out vec4 vertexColor;

uniform mat4 model;
uniform mat4 rotation;
uniform vec3 origin;
uniform vec3 ray;
uniform float size;

void main() {
  int ID = gl_VertexID;
  vec3 result = vec3(0);

  if (ID / 2 == 0) {
    result = origin;
  } else {
    result = ray;
  }

  vec3 temp = vec3(0);
  if (ID % 2 == 1) {
    temp.x += size / 2;
  } else {
    temp.x -= size / 2;
  }

  gl_Position = model * (vec4(result, 1.0) + rotation * vec4(temp, 0.0));
  vertexColor = vec4(1.0, 1.0, 1.0, 1.0);
}