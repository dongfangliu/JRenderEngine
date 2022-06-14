#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 WorldPos;
out vec4 LightSpaceFragPos;
out vec3 Normal;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


uniform mat4 lightSpaceMatrix;

void main()
{
    TexCoords = aTexCoords;
    WorldPos = vec3(model*vec4(aPos,1.0));
    Normal = mat3(model)*aNormal;
    TBN = mat3(mat3(model)*aTangent,mat3(model)*aBitangent,mat3(model)*aNormal);
    LightSpaceFragPos = lightSpaceMatrix * vec4(WorldPos,1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}