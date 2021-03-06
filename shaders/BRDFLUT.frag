#version 330 core

in vec2 TexCoords;

out vec2 FragColor;

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
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

float GeometrySchlickGGX(float cosTheta, float k)
{
    float nom   = cosTheta;
    float denom = cosTheta * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
//https://zhuanlan.zhihu.com/p/66518450

vec2 integrateBRDF(float NdotV,float roughness){
    //sin our geomerty function only cases about the cos value between N and V, N and L
    // so unnecessary to care about N's value, then we can assum N is (0,0,1),
    vec3 N = vec3(0,0,1);
    // also, V is actual value is not important, as long as NdotV is OK
    vec3 V = vec3(0);
    V.x = sqrt(1-NdotV*NdotV);
    V.y = 0;
    V.z = NdotV;


    float A = 0;
    float B = 0;
    float k = roughness*roughness/2;
    const uint sampleNum=1024u;

    for(uint i = 0u ;i<sampleNum;i++){
        vec2 Xi = Hammersley(i,sampleNum);
        vec3 H = ImportanceSamplingOnNDF(Xi,N,roughness);
        vec3 L = normalize(2.0*dot(V,H)*H-V);


        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL>0){
            float  G =  GeometrySmith( N,  V,  L,  k);
            float G_vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1-VdotH,5);
            A += (1-Fc)*G_vis;
            B += Fc*G_vis;
        }
    }
    return vec2(A,B)/sampleNum;
}

void main(){

    FragColor = integrateBRDF(TexCoords.x,TexCoords.y);
}

