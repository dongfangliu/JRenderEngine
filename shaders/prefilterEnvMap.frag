#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform samplerCube environmentMap;
uniform float roughness;
const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
float VanDerCorpus(uint n, uint base)
{
    float invBase = 1.0 / float(base);
    float denom   = 1.0;
    float result  = 0.0;

    for(uint i = 0u; i < 32u; ++i)
    {
        if(n > 0u)
        {
            denom   = mod(float(n), 2.0);
            result += denom * invBase;
            invBase = invBase / 2.0;
            n       = uint(float(n) / 2.0);
        }
    }

    return result;
}
// ----------------------------------------------------------------------------
vec2 HammersleyNoBitOps(uint i, uint N)
{
    return vec2(float(i)/float(N), VanDerCorpus(i, 2u));
}

vec3 ImportanceSamplingOnNDF(vec2 Xi, vec3 N, float roughness)
{
 float a = roughness*roughness;

 float phi = 2.0 * PI * Xi.x;
 float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
 float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

 // from spherical coordinates to cartesian coordinates
 vec3 H;
 H.x = cos(phi) * sinTheta;
 H.y = sin(phi) * sinTheta;
 H.z = cosTheta;

 // from tangent-space vector to world-space sample vector
 vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
 vec3 tangent   = normalize(cross(up, N));
 vec3 bitangent = cross(N, tangent);

 vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
 return normalize(sampleVec);
}

void main(){
    vec3 N = normalize(localPos);
    vec3 V = N;
    vec3 R = N;

    const uint sampleNum = 1024u;


    vec3 color = vec3(1,0,0);
    float totalWeight = 0;
    for(uint i = 0u;i<sampleNum;i++){
        vec2 Xi = HammersleyNoBitOps(i,sampleNum);
        vec3 H =  ImportanceSamplingOnNDF( Xi,  N, roughness);
        vec3 L = normalize(2.0*dot(V,H)*H-V);
        float NoL = max(dot(N,L),0.0);

        if(NoL>0){
           color+= texture(environmentMap,L).rgb*NoL;
           totalWeight+= NoL;
        }
    }
    color = color/totalWeight;
    FragColor = vec4(color,1.0f);


}