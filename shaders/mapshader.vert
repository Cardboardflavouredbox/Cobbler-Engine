#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;

void main() {
  gl_Position = model * vec4(aPos, 1.0);
  FragPos = aPos;
  Normal = aNormal;
  TexCoord = aTexCoord;
}