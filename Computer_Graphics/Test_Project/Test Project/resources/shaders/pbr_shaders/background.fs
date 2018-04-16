#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;
uniform samplerCube prefilterMap;
uniform int mipLevel;

void main()
{	
	vec3 envColor;

	if(mipLevel <= 0)
	{
		envColor = textureLod(environmentMap, WorldPos, mipLevel).rgb;	
	}
    if(mipLevel > 0)
    {
    	envColor = textureLod(prefilterMap, WorldPos, mipLevel).rgb;
    }
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    FragColor = vec4(envColor, 1.0);
}
