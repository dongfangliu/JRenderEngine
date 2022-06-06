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
    unsigned int VAO{};
    unsigned int materialIndex{};

    // constructor
    Mesh() = default;
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, unsigned int materialIndex);
    static std::shared_ptr<Mesh> GetUnitCubeInstance(){
      if(Mesh::UnitCubeInstance== nullptr){
        Mesh::UnitCubeInstance = make_shared<Mesh>();
        ComputeBox(Mesh::UnitCubeInstance->vertices,Mesh::UnitCubeInstance->indices,glm::vec3(1),false,false);
        Mesh::UnitCubeInstance->SetupGL();
      }

      return Mesh::UnitCubeInstance;
    };

    static std::shared_ptr<Mesh> GetUnitSphereInstance(){
      if(Mesh::UnitSphereInstance== nullptr){
        Mesh::UnitSphereInstance = make_shared<Mesh>();
        ComputeSphere(Mesh::UnitSphereInstance->vertices,Mesh::UnitSphereInstance->indices,1,32,false,false);
        Mesh::UnitSphereInstance->SetupGL();
      }
      return Mesh::UnitSphereInstance;
    };
  // initializes all the buffer objects/arrays
  void SetupGL();
    void Draw() const;
 private:
    // render data 
    unsigned int VBO{}, EBO{};

    static std::shared_ptr<Mesh> UnitCubeInstance,UnitSphereInstance;

};
#endif
