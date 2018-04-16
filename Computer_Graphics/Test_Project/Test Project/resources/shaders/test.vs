#version 330 core

struct dirLight 
{
	vec3 color;
	vec3 direction;
  
	float ambient;
};
struct pointLight
{
  vec3 color;
  vec3 position;

  float ambient;

  float ConstantAtt;
  float LinearAtt;
  float QuadraticAtt;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out vec3 TangentLightDir;
out vec3 TangentLightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
uniform dirLight sun_light;
uniform pointLight point_light;


void main()
{
	mat3 TBN;
	FragPos = vec3(model * vec4(aPos,1.0));
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model)))*aNormal;    

    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = normalize(normalMatrix * aNormal);
  	if (dot(cross(N, T), B) < 0.0)
  	{
  		T = T * -1.0;
  	}
  	TBN = transpose(mat3(T, B, N));

    
    
    //mat3 TBN = transpose(mat3(T, B, N));    

    TangentViewPos  = TBN * viewPos; 
    TangentFragPos  = TBN * FragPos;
    TangentLightDir = TBN * sun_light.direction;
    TangentLightPos = TBN * point_light.position;

    gl_Position =projection * view * vec4(FragPos, 1.0f);
}