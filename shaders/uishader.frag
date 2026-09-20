#version 330 core

uniform int textureexists;
uniform sampler2D TextureInput;
uniform vec4 baseColor;

in vec2 uvout;

out vec4 fragmentColor;

void main() {
  vec4 tempcolor;

  if (textureexists > 0)
    tempcolor = texture(TextureInput, uvout);
  else
    tempcolor = vec4(1, 1, 1, 1);

  tempcolor *= baseColor;
  if (tempcolor.a < 0.1)
    discard;
  fragmentColor = tempcolor;
}