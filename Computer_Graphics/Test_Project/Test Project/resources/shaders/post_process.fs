#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

float EDGE_THRESHOLD_MIN = 0.0312;
float EDGE_THRESHOLD_MAX = 0.125;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;

uniform bool bloom;
uniform bool chromatic_aberration;
uniform bool vignette;

uniform float exposure;
uniform float ca_offset;
uniform float ca_strength;
uniform vec2 screenTextureSize;

vec2 Shift = vec2(2.5,-0.5);
float Strength = 4.5;
vec3 col;

void Chromatic_Aberration()
{
	float amount = 0.0;
	
	amount = (1.0 + sin(6.0)) * 0.5;
	amount *= 1.0 + sin(16.0) * 0.5;
	amount *= 1.0 + sin(19.0) * 0.5;
	//amount *= 1.0 + sin(27.0) * 0.5;
	amount = pow(amount, 3.0);

	amount *= 0.05;
	
    col.r = texture( screenTexture, vec2(TexCoords.x+amount,TexCoords.y) ).r;
    col.g = texture( screenTexture, TexCoords ).g;
    col.b = texture( screenTexture, vec2(TexCoords.x-amount,TexCoords.y) ).b;

    col *= (1.0 - amount * 0.5);
}
vec3 Chromatic_Aberration2()
{
	vec3 color, colorInput = texture(screenTexture, TexCoords).rgb;
	// Sample the color components
	vec2 distance_xy = TexCoords - vec2(0.5f);

	// Adjust the ratio
	distance_xy *= vec2(1600/900, 1.6f);

	// Calculate the distance
	distance_xy /= 2.0f;
	float distance = dot(distance_xy, distance_xy);

	float ca_offset_temp =ca_offset * (1.0 + pow(distance, 2.0 * 0.5) * -2.0);
	float temp_ca_strenght = ca_strength * (1.0 + pow(distance, 2.0 * 0.5) * -2.0);

	color.r = texture(screenTexture, TexCoords + (vec2(0.000625,0.0011) * vec2(ca_offset_temp,-0.5f))).r;
	color.g = colorInput.g;
	color.b = texture(screenTexture, TexCoords - (vec2(0.000625,0.0011) * vec2(ca_offset_temp,-0.5f))).b;

	// Adjust the strength of the effect
	return mix(colorInput, color, ca_strength);
}

void Vignette()
{
	vec2 distance_xy = TexCoords - vec2(0.5f);

	// Adjust the ratio
	distance_xy *= vec2(1600/900, 1.6f);

	// Calculate the distance
	distance_xy /= 2.0f;
	float distance = dot(distance_xy, distance_xy);

	// Apply the vignette
	col.rgb *= (1.0 + pow(distance, 2.0 * 0.5) * -2.0); //pow - multiply
}

void Tone_Mapping()
{
	float Y = dot(vec3(0.30, 0.59, 0.11), col);
    float YD = exposure * (exposure/2 + 1.0) / (exposure + 1.0);
    col = col * YD;
}
void main()
{             
    col = texture(screenTexture, TexCoords).rgb;      
    //col = computeFxaa();


 	if(chromatic_aberration)
 	{
		col = Chromatic_Aberration2();
 	}

    if(vignette)
    {
    	Vignette();	
    }
    Tone_Mapping();
    
   
  
 	FragColor = vec4(col, 1.0);
}