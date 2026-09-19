#version 330 core
// https://gamedev.stackexchange.com/a/60377
uniform mat4 uProjectionMatrix;
uniform mat4 uWorldToCameraMatrix;
uniform mat4 uOrientmat;

layout(location = 0) in vec2 aPosition;

smooth out vec3 eyeDirection;

void main() {
  mat4 inverseProjection = inverse(uProjectionMatrix);
  mat3 inverseModelview = transpose(mat3(uWorldToCameraMatrix));
  vec3 unprojected = (inverseProjection * vec4(aPosition, 0, 1)).xyz;
  eyeDirection = (uOrientmat * vec4(inverseModelview * unprojected, 1.0)).xyz;

  gl_Position = vec4(aPosition, 0, 1);
}