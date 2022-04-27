
#pragma  once
#include <utility>
#include <vector>
#include "glm/glm.hpp"
// reference to
//https://www.danielsieger.com/blog/2021/03/27/generating-spheres.html
//https://www.danielsieger.com/blog/2021/05/03/generating-primitive-shapes.html
enum ShapeType {
  Sphere, Box, Cylinder, Mesh
  //, Cone,Plane, Torus
};
class BaseMeshShape {
 public:
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<unsigned int> indices;
  ShapeType type=ShapeType::Mesh;
  glm::vec3 AddVertex(double x, double y, double z) {
    auto v = glm::vec3(x, y, z);
    this->positions.push_back(v);
    return v;
  }
  BaseMeshShape() = default;
  ~BaseMeshShape() = default;
  void AddTriangle(unsigned int i0, unsigned int i1, unsigned int i2) {
    this->indices.push_back(i0);
    this->indices.push_back(i1);
    this->indices.push_back(i2);
  }
  void AddQuad(unsigned int i0,
               unsigned int i1,
               unsigned int i2,
               unsigned int i3) { //CLOCK WISE
    this->indices.push_back(i0);
    this->indices.push_back(i1);
    this->indices.push_back(i2);
    this->indices.push_back(i0);
    this->indices.push_back(i2);
    this->indices.push_back(i3); }
  BaseMeshShape(std::vector<glm::vec3> positions,
                std::vector<glm::vec3> normals,
                std::vector<unsigned int> indices) : positions(std::move(positions)), normals(std::move(normals)), indices(std::move(indices)) {}
};

class UnitSphereMeshShape : public BaseMeshShape {
 public:
  int n_slices, n_stacks;
  explicit UnitSphereMeshShape(int n_slices = 16, int n_stacks = 16);
};

class CylinderMeshShape : public BaseMeshShape {
 public:
  double radius;
  double height;
  CylinderMeshShape(double radius, double height, std::size_t resolution = 32);
};
class UnitBoxMeshShape : public BaseMeshShape {
 public:
  explicit UnitBoxMeshShape();
 private:
  constexpr static float cubeNormals[]={
      // Back face
      0,0,-1,
      0,0,-1,
      0,0,-1,
      0,0,-1,
      0,0,-1,
      0,0,-1,
      // Front face

      0,0,1,
      0,0,1,
      0,0,1,
      0,0,1,
      0,0,1,
      0,0,1,
      // Left face
      -1,0,0,
      -1,0,0,
      -1,0,0,
      -1,0,0,
      -1,0,0,
      -1,0,0,
      // Right face
      1,0,0,
      1,0,0,
      1,0,0,
      1,0,0,
      1,0,0,
      1,0,0,
      // Bottom face
      0,-1,0,
      0,-1,0,
      0,-1,0,
      0,-1,0,
      0,-1,0,
      0,-1,0,
      // Top face
      0,1,0,
      0,1,0,
      0,1,0,
      0,1,0,
      0,1,0,
      0,1,0,
  };
  constexpr  static float cubeVertices[] = {
// Back face
      -0.5f, -0.5f, -0.5f,    // Bottom-left
      0.5f,  0.5f, -0.5f,    // top-right
      0.5f, -0.5f, -0.5f,    // bottom-right
      0.5f,  0.5f, -0.5f,    // top-right
      -0.5f, -0.5f, -0.5f,    // bottom-left
      -0.5f,  0.5f, -0.5f,    // top-left
      // Front face
      -0.5f, -0.5f,  0.5f,    // bottom-left
      0.5f, -0.5f,  0.5f,    // bottom-right
      0.5f,  0.5f,  0.5f,    // top-right
      0.5f,  0.5f,  0.5f,    // top-right
      -0.5f,  0.5f,  0.5f,    // top-left
      -0.5f, -0.5f,  0.5f,    // bottom-left
      // Left face
      -0.5f,  0.5f,  0.5f,    // top-right
      -0.5f,  0.5f, -0.5f,    // top-left
      -0.5f, -0.5f, -0.5f,    // bottom-left
      -0.5f, -0.5f, -0.5f,    // bottom-left
      -0.5f, -0.5f,  0.5f,    // bottom-right
      -0.5f,  0.5f,  0.5f,    // top-right
      // Right face
      0.5f,  0.5f,  0.5f,    // top-left
      0.5f, -0.5f, -0.5f,    // bottom-right
      0.5f,  0.5f, -0.5f,    // top-right
      0.5f, -0.5f, -0.5f,    // bottom-right
      0.5f,  0.5f,  0.5f,    // top-left
      0.5f, -0.5f,  0.5f,    // bottom-left
      // Bottom face
      -0.5f, -0.5f, -0.5f,    // top-right
      0.5f, -0.5f, -0.5f,    // top-left
      0.5f, -0.5f,  0.5f,    // bottom-left
      0.5f, -0.5f,  0.5f,    // bottom-left
      -0.5f, -0.5f,  0.5f,    // bottom-right
      -0.5f, -0.5f, -0.5f,    // top-right
      // Top face
      -0.5f,  0.5f, -0.5f,    // top-left
      0.5f,  0.5f,  0.5f,    // bottom-right
      0.5f,  0.5f, -0.5f,    // top-right
      0.5f,  0.5f,  0.5f,    // bottom-right
      -0.5f,  0.5f, -0.5f,    // top-left
      -0.5f,  0.5f,  0.5f,     // bottom-left
  };
};
