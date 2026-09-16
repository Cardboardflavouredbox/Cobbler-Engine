#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 TexCoord;

uniform mat4 particlemodel;
uniform mat4 particlerotation;
uniform vec3 BillboardPosition;
uniform vec2 BillboardSize;
uniform vec2 uvOffset;
uniform vec2 uvSize;

void main()
{	
	vec3 vertexPosition_worldspace = vec3(1,0,0) * aPos.x * BillboardSize.x
		+ vec3(0,0,1) * aPos.y * BillboardSize.y;

    gl_Position = particlemodel * (vec4(BillboardPosition, 0.0f) + particlerotation * vec4(vertexPosition_worldspace, 1.0f));
    vertexColor = vec4(1.0, 1.0, 1.0, 1.0);
    TexCoord = aTexCoord * uvSize + uvOffset;
}