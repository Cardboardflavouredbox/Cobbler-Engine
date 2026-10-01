#version 330 core

in vec4 vertexColor;

out vec4 fragmentColor;

void main() {
  if (vertexColor.a < 0.1)
    discard;
  fragmentColor = vertexColor;
}