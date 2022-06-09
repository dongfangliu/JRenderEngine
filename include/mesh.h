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
    static std::shared_ptr<Mesh> GetUnitQuadInstance(){
    if(Mesh::UnitSphereInstance== nullptr){
      Mesh::UnitQuadInstance = make_shared<Mesh>();
      Mesh::UnitQuadInstance->vertices.push_back({glm::vec3(1.0f,  1.0f, 0.0f),glm::vec3(0,0,-1),glm::vec2(1,1)});
      Mesh::UnitQuadInstance->vertices.push_back({glm::vec3(1.0f,  -1.0f, 0.0f),glm::vec3(0,0,-1),glm::vec2(1,0)});
      Mesh::UnitQuadInstance->vertices.push_back({glm::vec3(-1.0f, -1.0f, 0.0f),glm::vec3(0,0,-1),glm::vec2(0,0)});
      Mesh::UnitQuadInstance->vertices.push_back({glm::vec3(-1.0f,  1.0f, 0.0f),glm::vec3(0,0,-1),glm::vec2(0,1)});
      Mesh::UnitQuadInstance->indices.push_back(0);
      Mesh::UnitQuadInstance->indices.push_back(1);
      Mesh::UnitQuadInstance->indices.push_back(3);
      Mesh::UnitQuadInstance->indices.push_back(1);
      Mesh::UnitQuadInstance->indices.push_back(2);
      Mesh::UnitQuadInstance->indices.push_back(3);
      Mesh::UnitQuadInstance->SetupGL();
    }
    return Mesh::UnitQuadInstance;
  };
  // initializes all the buffer objects/arrays
  void SetupGL();
    void Draw() const;
 private:
    // render data 
    unsigned int VBO{}, EBO{};

    static std::shared_ptr<Mesh> UnitCubeInstance,UnitSphereInstance,UnitQuadInstance;

};
#endif
