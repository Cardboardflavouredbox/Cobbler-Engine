#version 330 core

// in vec4 vertexColor;
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

in vec2 TexCoord;

uniform sampler2D InputTexture;
uniform int lightcnt;
uniform vec3 lightPos[16];
uniform vec3 lightColor[16];
uniform float specularStrength[16];
uniform float diffusionStrength[16];
uniform vec3 viewPos;

void main() {
  vec4 tempcolor = texture(InputTexture, vec2(TexCoord.x, 1 - TexCoord.y));
  if (tempcolor.a < 0.1)
    discard;
  vec3 result = vec3(0, 0, 0);
  for (int i = 0; i < 16; i++) {
    if (i >= lightcnt)
      break;
    vec3 Color = lightColor[i];

    int shininess = 16;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos[i] - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffusionStrength[i] * Color;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength[i] * spec * Color;

    result += (diffuse + specular) * tempcolor.xyz;
  }
  FragColor = vec4(result, 1.0);
}