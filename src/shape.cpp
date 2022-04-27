#include "shape.h"
UnitSphereMeshShape::UnitSphereMeshShape( int n_slices, int n_stacks)
    :  n_slices(n_slices), n_stacks(n_stacks) {
  this->type = ShapeType::Sphere;

  double endU=3.14*2;
  double endV=3.14;
  double stepU=(endU)/n_stacks;// step size between U-points on the grid
  double stepV=(endV)/n_slices;// step size between V-points on the grid
  for(int i=0;i<n_stacks;i++){ // U-points
    for(int j=0;j<n_slices;j++){ // V-points
      double u=i*stepU;
      double v=j*stepV;
      double un=(i+1==n_stacks) ? endU : (i+1)*stepU;
      double vn=(j+1==n_slices) ? endV : (j+1)*stepV;
      // Find the four points of the grid
      // square by evaluating the parametric
      // surface function
      glm::vec3 p0=glm::vec3(std::cos(u)*std::sin(v),std::cos(v),std::sin(u)*std::sin(v));
      glm::vec3 p1=glm::vec3(std::cos(u)*std::sin(vn),std::cos(vn),std::sin(u)*std::sin(vn));
      glm::vec3 p2=glm::vec3(std::cos(un)*std::sin(v),std::cos(v),std::sin(un)*std::sin(v));
      glm::vec3 p3=glm::vec3(std::cos(un)*std::sin(vn),std::cos(vn),std::sin(un)*std::sin(vn));
      // NOTE: For spheres, the normal is just the normalized
      // version of each vertex point; this generally won't be the case for
      // other parametric surfaces.
      // Output the first triangle of this grid square
      unsigned int offset = this->positions.size();
      this->positions.push_back(p0);
      this->positions.push_back(p1);
      this->positions.push_back(p2);
      this->positions.push_back(p3);
//	  triangle(p0, p2, p1)
      this->indices.push_back(offset);
      this->indices.push_back(offset+2);
      this->indices.push_back(offset+1);
//	  triangle(p3, p1, p2)
      this->indices.push_back(offset+3);
      this->indices.push_back(offset+1);
      this->indices.push_back(offset+2);
    }
  }
  this->normals = this->positions;
}
CylinderMeshShape::CylinderMeshShape(double radius, double height, std::size_t resolution) : radius(radius), height(height) {

  this->type = ShapeType::Cylinder;
  // generate vertices
  std::vector<glm::vec3> bottom_vertices;
  std::vector<glm::vec3> top_vertices;
  for (size_t i = 0; i < resolution; i++) {
    double ratio = static_cast<double>(i) / (resolution);
    double r = ratio * (3.14 * 2.0);
    double x = std::cos(r) * radius;
    double y = std::sin(r) * radius;
    glm::vec3 v = AddVertex(x, y, -height / 2);
    this->normals.push_back(glm::vec3(0,0,-1));
    bottom_vertices.push_back(v);
    v = AddVertex(x, y, height / 2);
    this->normals.push_back(glm::vec3(0,0,1));
    top_vertices.push_back(v);
  }
  // add bottom and top triangle faces
  //first add center vertex of faces
  unsigned int tVId = this->positions.size();
  AddVertex(0, 0, height / 2);
  this->normals.push_back(glm::vec3(0,0,1));
  unsigned int bVId = this->positions.size();
  AddVertex(0, 0, -height / 2);
  this->normals.push_back(glm::vec3(0,0,-1));
  // then add faces
  for (unsigned int i = 0; i < resolution; i++) {
    if (i != resolution - 1) {
      AddTriangle(i * 2, bVId, (i + 1) * 2);
      AddTriangle(i * 2 + 1, (i + 1) * 2 + 1, tVId);
    } else {
      AddTriangle(i * 2, bVId, 0);
      AddTriangle(i * 2 + 1, 1, tVId);
    }
  }
  unsigned int side_vertices_offset = this->positions.size();
  //add side vertices, same as before ,but normal is changed.
  for (size_t i = 0; i < resolution; i++) {
    double ratio = static_cast<double>(i) / (resolution);
    double r = ratio * (3.14 * 2.0);
    double x = std::cos(r) * radius;
    double y = std::sin(r) * radius;
    glm::vec3 v = AddVertex(x, y, -height / 2);
    this->normals.push_back(glm::vec3(std::cos(r),std::sin(r) ,0));
    bottom_vertices.push_back(v);
    v = AddVertex(x, y, height / 2);
    this->normals.push_back(glm::vec3(std::cos(r),std::sin(r) ,0));
    top_vertices.push_back(v);
  }
  // add faces around the cylinder
  for (size_t i = 0; i < resolution; i++) {
    auto ii = i * 2;
    auto jj = (ii + 2) % (resolution * 2);
    auto kk = (ii + 3) % (resolution * 2);
    auto ll = ii + 1;
    AddQuad(ii+side_vertices_offset, jj+side_vertices_offset, kk+side_vertices_offset, ll+side_vertices_offset);
  }


}
UnitBoxMeshShape::UnitBoxMeshShape() {

  this->type = ShapeType::Box;

  for(int i = 0;i<36;i++){
    glm::vec3 origin_vertex = glm::vec3(UnitBoxMeshShape::cubeVertices[3*i], UnitBoxMeshShape::cubeVertices[3*i+1], UnitBoxMeshShape::cubeVertices[3*i+2]);
    this->normals.push_back( glm::vec3(UnitBoxMeshShape::cubeNormals[3*i], UnitBoxMeshShape::cubeNormals[3*i+1], UnitBoxMeshShape::cubeNormals[3*i+2]));
    this->positions.push_back(origin_vertex);
    this->indices.push_back(i);
  }
}