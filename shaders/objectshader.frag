#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D InputTexture;

void main()
{
    vec4 tempcolor = texture(InputTexture, vec2(TexCoord.x,1-TexCoord.y));
    if(tempcolor.a < 0.1)
        discard;
    FragColor = tempcolor;
}