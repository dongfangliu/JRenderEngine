#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform samplerCube environmentMap;


const float PI = 3.14159265359;

void main(){
    vec3 irradiance=vec3(0);
    vec3 normal = normalize(localPos);

    vec3 yAxis  = vec3(0,1,0);

    vec3 xAxis = normalize(cross(yAxis,normal));
    yAxis =  normalize(cross(normal,xAxis));

    float sampleDelta = 0.025f;
    float nSamples = 0;
    for(float phi = 0;phi<=2*PI;phi+=sampleDelta){
        for(float theta = 0;theta<=PI/2;theta+=sampleDelta){
            // spherical to cartisan ,z-up axis
            vec3 sphere_coord = vec3(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta));
            // rotate to make z-up axis along normal direction
            vec3 sampleDir = sphere_coord.x*xAxis+sphere_coord.y*yAxis+sphere_coord.z*normal;
            irradiance+=texture(environmentMap,sampleDir).rgb*cos(theta)*sin(theta);
            nSamples++;
        }
    }
    irradiance= PI*irradiance*(1.0/float(nSamples));
    FragColor = vec4(irradiance, 1.0);


}