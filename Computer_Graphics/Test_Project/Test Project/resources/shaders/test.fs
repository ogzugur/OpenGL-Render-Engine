#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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

vec3 calculate_dir_light(dirLight dir_light);
vec3 calculate_point_light(pointLight point_light);
vec4 sample_from_texture();

//out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec3 TangentLightDir;
in vec3 TangentLightPos;

uniform bool is_directional_light_on;
uniform int texture_channel;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;
uniform sampler2D depthMap;
uniform float far_plane;

uniform dirLight sun_light;
uniform pointLight point_light;
uniform vec3 viewPos;
uniform float bloom_threshold;


void main()
{   
	vec3 final_color;

	if(is_directional_light_on)
	{
		final_color = calculate_point_light(point_light);	
	}
	else
	{
		final_color = sample_from_texture().xyz;
	}
	
	float brightness = dot(final_color, vec3(0.2126, 0.7152, 0.0722))*1.5;
    if(brightness > bloom_threshold)
    {
        BrightColor = vec4(final_color, 1.0);
    }
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    FragColor = vec4(final_color, 1.0);
    
}

vec3 calculate_dir_light(dirLight dir_light)
{
	//ambient
	vec3 ambient = dir_light.ambient * sample_from_texture().rgb;

	//diffuse


	vec3 normal = texture(texture_normal1, TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);


	vec3 lightDir = normalize(-TangentLightDir);
	float diff = max(dot(normal,lightDir), 0.0);
	vec3 diffuse = dir_light.color * diff * sample_from_texture().rgb;

	//specular
	vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
    vec3 specular = vec3(0.5f, 0.5f, 0.5f) * spec * texture(texture_specular1, TexCoords).rgb;



	vec3 final_color = ambient + diffuse + specular;

	return final_color;
}

vec3 calculate_point_light(pointLight point_light)
{
	  //normal
	 vec3 color = texture(texture_diffuse1, TexCoords).rgb;

	vec3 normal = texture(texture_normal1, TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

	 // ambient
    vec3 ambient = sample_from_texture().rgb;

    //diffuse
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(normal,lightDir), 0.0);
	vec3 diffuse = point_light.color * diff * sample_from_texture().rgb;

	//specular
	vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
    vec3 specular = vec3(0.9f, 0.9f, 0.9f) * spec * texture(texture_specular1, TexCoords).rgb;

    //attenuation
    float distance    = length(TangentLightPos - TangentFragPos);
    float attenuation = 1.0 / (point_light.ConstantAtt + point_light.LinearAtt * distance + point_light.QuadraticAtt * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   

    vec3 final_color = ambient+specular+diffuse;
    

 	 return final_color;
}

vec4 sample_from_texture()
{
	if(texture_channel == 0)
   		return texture(texture_diffuse1,TexCoords);
   	else if(texture_channel == 1)
   		return texture(texture_normal1,TexCoords);
   	else if(texture_channel == 2)
   		return texture(texture_specular1,TexCoords);

   	return texture(texture_diffuse1,TexCoords);
}