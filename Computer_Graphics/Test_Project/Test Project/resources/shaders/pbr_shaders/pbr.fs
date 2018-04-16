#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 envMapCoords;
// material parameters
uniform sampler2D gPosition;
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gEffects;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;
uniform int gBufferView;
uniform float bloom_threshold;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   
// ----------------------------------------------------------------------------
vec3 ReinhardTM(vec3 color)
{
    return color / (color + vec3(1.0f));
}

vec2 getSphericalCoord(vec3 normalCoord)
{
    float phi = acos(-normalCoord.y);
    float theta = atan(1.0f * normalCoord.x, -normalCoord.z) + PI;

    return vec2(theta / (2.0f * PI), phi / PI);
}

void main()
{	
	vec3 color = vec3(1.0f);
    // material properties
 	vec4 colorOutput = vec4(0.0f);
    vec3 viewPos = texture(gPosition, TexCoords).rgb;
    vec3 albedo = pow(texture(gAlbedo, TexCoords).rgb, vec3(2.2));
    vec3 normal = texture(gNormal, TexCoords).rgb;
    float metallic = texture(gNormal, TexCoords).a;
    float roughness = texture(gAlbedo, TexCoords).a;
    float ao = texture(gEffects, TexCoords).r;
    float depthValue = texture(gPosition, TexCoords).a/1000.0f;
       
    //vec3 envColor = texture(prefilterMap, getSphericalCoord(normalize(envMapCoords))).rgb;

     if(depthValue >= 0.0 && depthValue <= 0.001)
     {
        //	color = texture(prefilterMap,vec3(TexCoords,1.0)).rgb;
        //color = envColor;
     }
     else
     {
	// input lighting data
    vec3 N = normalize(normal);
    vec3 V = normalize(-viewPos);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - viewPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - viewPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);    
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	                
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    
    	color = ambient + Lo;


     }
    
    // HDR tonemapping
     color = ReinhardTM(color);

    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    if(gBufferView == 1)
        colorOutput = vec4(color, 1.0f);

    // Position buffer
    else if (gBufferView == 2)
        colorOutput = vec4(vec3(depthValue), 1.0f);

    // View Normal buffer
    else if (gBufferView == 3)
        colorOutput = vec4(normal, 1.0f);

    // Color buffer
    else if (gBufferView == 4)
        colorOutput = vec4(albedo, 1.0f);

    // Roughness buffer
    else if (gBufferView == 5)
        colorOutput = vec4(vec3(roughness), 1.0f);

    // Metalness buffer
    else if (gBufferView == 6)
        colorOutput = vec4(vec3(metallic), 1.0f);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722))*1.5;
    if(brightness > bloom_threshold)
    {
        BrightColor = vec4(color, 1.0);
    }
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    FragColor = colorOutput;
}


