
#pragma  once
#include <utility>
#include <vector>
#include "glm/glm.hpp"
struct Vertex {
  // position
  glm::vec3 Position;
  // normal
  glm::vec3 Normal;
  // texCoords
  glm::vec2 TexCoords;
  // tangent
  glm::vec3 Tangent;
  // bitangent
  glm::vec3 Bitangent;

};
void ComputeTangentBasis(
    std::vector<Vertex> &vertices,
    std::vector<unsigned int> &indices
);
void ComputeBox(std::vector<Vertex> &vertices,
                std::vector<unsigned int> &indices,
                const glm::vec3 &size,
                bool rhcoords,
                bool invertn);
void ComputeSphere(std::vector<Vertex> &vertices,
                   std::vector<unsigned int> &indices,
                   float diameter,
                   size_t tessellation,
                   bool rhcoords,
                   bool invertn);
void ComputeGeoSphere(std::vector<Vertex> &vertices,
                      std::vector<unsigned int> &indices,
                      float diameter,
                      size_t tessellation,
                      bool rhcoords);
void ComputeCylinder(std::vector<Vertex> &vertices,
                     std::vector<unsigned int> &indices,
                     float height,
                     float diameter,
                     size_t tessellation,
                     bool rhcoords);
void ComputeCone(std::vector<Vertex> &vertices,
                 std::vector<unsigned int> &indices,
                 float diameter,
                 float height,
                 size_t tessellation,
                 bool rhcoords);
void ComputeTorus(std::vector<Vertex> &vertices,
                  std::vector<unsigned int> &indices,
                  float diameter,
                  float thickness,
                  size_t tessellation,
                  bool rhcoords);
void ComputeTetrahedron(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, float size, bool rhcoords);
void ComputeOctahedron(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, float size, bool rhcoords);
void ComputeDodecahedron(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, float size, bool rhcoords);
void ComputeIcosahedron(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, float size, bool rhcoords);
void ComputeTeapot(std::vector<Vertex> &vertices,
                   std::vector<unsigned int> &indices,
                   float size,
                   size_t tessellation,
                   bool rhcoords);