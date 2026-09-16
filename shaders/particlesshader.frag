#version 330 core

in vec4 vertexColor;
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D InputTexture;

void main()
{
    vec4 tempcolor = texture(InputTexture, TexCoord) * vertexColor;
    if(tempcolor.a < 0.1)
        discard;
    FragColor = tempcolor;
}