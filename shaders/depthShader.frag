#version 330 core
out vec4 FragColor;
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * 0.1f * 1000.0f) / (1000.0f + 0.1f - z * (1000.0f - 0.1f));
}
void main()
{
    //FragColor = vec4(vec3(LinearizeDepth(gl_FragCoord.z)/1000.0f), 1.0);
}