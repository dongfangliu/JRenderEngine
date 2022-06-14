#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec4 LightSpaceFragPos;
in vec3 Normal;
in mat3 TBN;

uniform sampler2D baseColorMap;
uniform sampler2D normalMap;
uniform sampler2D metallicRoughnessMap;

uniform samplerCube diffuseIrradianceMap;
uniform samplerCube prefilteredMap;
uniform sampler2D BRDFLUT;
uniform sampler2D depthMap;
//uniform sampler2D aoMap;

uniform vec4 baseColorFactor;
uniform float metallicFactor;
uniform float roughnessFactor;

uniform int baseColorMode;
uniform int metallicMode;
uniform int alphaMode ;
uniform float alphaCutOff;
uniform int roughnessMode;
uniform bool useNormalMap;


uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

const float PI = 3.14159265359;


vec3 getNormalFromMap(){
    vec3 tangentNormal = texture(normalMap,TexCoords).xyz*2.0-1.0;

    return normalize(TBN*tangentNormal);
}
vec4 getBaseColor(){
    vec4 color = vec4(1);
    if(baseColorMode==2){
        color =  texture(baseColorMap,TexCoords).rgba;
    }else if(baseColorMode == 4){
        color = baseColorFactor;
    }else if(baseColorMode ==6){
        color = baseColorFactor*texture(baseColorMap,TexCoords).rgba;
    }
    return vec4(pow(color.rgb,vec3(2.2)),color.a);
}

float getMetallic(){
    float m = 0;
    if(metallicMode==2){
        m =  texture(metallicRoughnessMap,TexCoords).b;
    }else if(metallicMode == 4){
        m = metallicFactor;
    }else if(metallicMode ==6){
        m = metallicFactor*texture(baseColorMap,TexCoords).b;
    }
    return m;
}
float getRoughness(){    
float m = 0;
    if(roughnessMode==2){
        m =  texture(metallicRoughnessMap,TexCoords).g;
    }else if(roughnessMode == 4){
        m = roughnessFactor;
    }else if(roughnessMode ==6){
        m = roughnessFactor*texture(metallicRoughnessMap,TexCoords).g;
    }
    return m;

}

float D_GGX_TR(vec3 N,vec3 H,float a ){
    float a2 = a *a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;

    return nom / denom;
}

float GGX_Schlick(vec3 n, vec3 v,float k){
    float NdotV = max(dot(n,v),0);
    return NdotV/(NdotV*(1-k)+k);

}

float G_Smith(vec3 N, vec3 V,vec3 L,float k){
    return GGX_Schlick(N,V,k)*GGX_Schlick(N,L,k);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float ShadowCalculation()
{
    float shadow =0.0f;
    vec3 projCoords = LightSpaceFragPos.xyz / LightSpaceFragPos.w;
    projCoords = projCoords*0.5+0.5;
    //float bias =0;
    float bias = max(0.05 * (1.0 - dot(Normal, normalize(lightPositions[0]-WorldPos))), 0.005);
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    if(projCoords.z > 1.0){
          return shadow;
    }
    float closestDepth = texture(depthMap, projCoords.xy).r;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    for(int x = -1; x <= 1; ++x)
    {
         for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
       }
    }
    shadow /= 9.0;
    return shadow;
}

void main()
{
    vec4 baseColor     =getBaseColor();
    float metallic  = getMetallic();
    float roughness = getRoughness();
   // float ao        = texture(aoMap, TexCoords).r;
    if(alphaMode==1 && baseColor.a<alphaCutOff){
        discard;
    }
    vec3 albedo = baseColor.rgb;

    vec3 N ;
    if(useNormalMap){
        N =getNormalFromMap();
    }else{
        N = Normal;
    }
    vec3 V = normalize(camPos - WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the baseColor color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        //float attenuation = 1.0 / (distance * distance);
        float attenuation=1.0f;
        vec3 radiance = lightColors[i] * attenuation;
        // Cook-Torrance BRDF
        float NDF = D_GGX_TR(N, H, roughness);
        float k =  (roughness+1)*(roughness+1)/8;
        float G   = G_Smith(N, V, L, k);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);


        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   

    vec3 color = vec3(0);
    vec3 IBL_Color = vec3(0);
    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    // add IBL Diffuse part
    IBL_Color+= kD*texture(diffuseIrradianceMap,N).rgb*albedo;
    // add IBL specular part
    vec3 R= reflect(-V,N);
    const int maxMipIndex = 4;
    vec3 prefilteredEnvColor = textureLod(prefilteredMap, R,  roughness * maxMipIndex).rgb;
    vec2 lut = texture(BRDFLUT,vec2(max(dot(N,V),0),roughness)).rg;
    vec3 specular = prefilteredEnvColor*(F0*lut.r+lut.g);
    IBL_Color += kS*specular;

    color=Lo*(1-ShadowCalculation())+IBL_Color;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}