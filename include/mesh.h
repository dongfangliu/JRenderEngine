#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "geometry.h"
#include "shader.h"
#include "texture.h"
#include <string>
#include <vector>
using namespace std;



class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    unsigned int VAO;
    unsigned int materialIndex; 

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, unsigned int materialIndex);

    // render the mesh
    void Draw(Shader shader);

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void SetupGL();
};
#endif
