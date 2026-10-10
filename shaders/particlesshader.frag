#version 330 core

in vec4 vertexColor;
out vec4 FragColor;

in vec2 TexCoord;

uniform int HasTexture;
uniform sampler2D InputTexture;

void main() {
  vec4 tempcolor;
  if (HasTexture > 0) {
    tempcolor = texture(InputTexture, TexCoord) * vertexColor;
  } else {
    tempcolor = vertexColor;
  }
  if (tempcolor.a < 0.1)
    discard;
  FragColor = tempcolor;
}