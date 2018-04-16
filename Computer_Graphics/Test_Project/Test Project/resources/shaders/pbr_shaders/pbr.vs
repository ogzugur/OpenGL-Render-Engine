#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;


out vec2 TexCoords;
out vec3 envMapCoords;

uniform mat4 inverseView;
uniform mat4 inverseProj;

void main()
{
  	TexCoords = aTexCoords;
  	vec4 unprojCoords = (inverseProj * vec4(aPos, vec2(1.0f)));
    envMapCoords = (inverseView * unprojCoords).xyz;
    
    gl_Position = vec4(aPos, 1.0);
}