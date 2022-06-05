#include <stdexcept>
#include <map>
#include "geometry.h"
#include "glm/gtx/transform.hpp"

constexpr float SQRT2 = 1.41421356237309504880f;
constexpr float SQRT3 = 1.73205080756887729352f;
constexpr float SQRT6 = 2.44948974278317809820f;
constexpr float Shape_PI = 3.141592654f;
constexpr float Shape_2PI = 6.283185307f;
constexpr float Shape_PIDIV2 = 1.570796327f;

inline void CheckIndexOverflow(size_t value) {
  // Use >=, not > comparison, because some D3D level 9_x hardware does not support 0xFFFF index values.
  if (value >= USHRT_MAX)
    throw std::out_of_range("Index value out of range: cannot tesselate primitive so finely");
}

// Collection types used when generating the geometry.
inline void index_push_back(std::vector<unsigned int> &indices, size_t value) {
  CheckIndexOverflow(value);
  indices.push_back(static_cast<unsigned int>(value));
}

// Helper for flipping winding of geometric primitives for LH vs. RH coords
inline void ReverseWinding(std::vector<unsigned int> &indices, std::vector<Vertex> &vertices) {
  assert((indices.size() % 3) == 0);
  for (auto it = indices.begin(); it != indices.end(); it += 3) {
    std::swap(*it, *(it + 2));
  }

  for (auto &it : vertices) {
    it.TexCoords.x = (1.f - it.TexCoords.x);
  }
}

// Helper for inverting normals of geometric primitives for 'inside' vs. 'outside' viewing
inline void InvertNormals(std::vector<Vertex> &vertices) {
  for (auto &it : vertices) {
    it.Normal.x = -it.Normal.x;
    it.Normal.y = -it.Normal.y;
    it.Normal.z = -it.Normal.z;
  }
}
inline void XMScalarSinCos(float *sin, float *cos, float radians) {
  *sin = std::sin(radians);
  *cos = std::cos(radians);
}

//--------------------------------------------------------------------------------------
// Cube (aka a Hexahedron) or Box
//--------------------------------------------------------------------------------------
void ComputeBox(std::vector<Vertex> &vertices,
                std::vector<unsigned int> &indices,
                glm::vec3 &size,
                bool rhcoords,
                bool invertn) {
  vertices.clear();
  indices.clear();

  // A box has six faces, each one pointing in a different direction.
  constexpr int FaceCount = 6;

  static const glm::vec4 faceNormals[FaceCount] =
      {
          {0, 0, 1, 0},
          {0, 0, -1, 0},
          {1, 0, 0, 0},
          {-1, 0, 0, 0},
          {0, 1, 0, 0},
          {0, -1, 0, 0},
      };

  static const glm::vec4 TexCoordss[4] =
      {
          {1, 0, 0, 0},
          {1, 1, 0, 0},
          {0, 1, 0, 0},
          {0, 0, 0, 0},
      };

  glm::vec3 tsize = size / 2.0f;

  // Create each face in turn.
  for (int i = 0; i < FaceCount; i++) {
    const glm::vec3 normal = faceNormals[i];

    // Get two vectors perpendicular both to the face normal and to each other.
    const glm::vec3 basis = (i >= 4) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);

    const glm::vec3 side1 = glm::cross(normal, basis);
    const glm::vec3 side2 = glm::cross(normal, side1);

    // Six indices (two triangles) per face.
    const size_t vbase = vertices.size();
    index_push_back(indices, vbase + 0);
    index_push_back(indices, vbase + 1);
    index_push_back(indices, vbase + 2);

    index_push_back(indices, vbase + 0);
    index_push_back(indices, vbase + 2);
    index_push_back(indices, vbase + 3);

    // Four vertices per face.
    // (normal - side1 - side2) * tsize // normal // t0
    vertices.push_back({(normal - side1 - side2) * tsize, normal, TexCoordss[0]});

    // (normal - side1 + side2) * tsize // normal // t1
    vertices.push_back({(normal - side1 + side2) * tsize, normal, TexCoordss[1]});

    // (normal + side1 + side2) * tsize // normal // t2
    vertices.push_back({(normal + side1 + side2) * tsize, normal, TexCoordss[2]});

    // (normal + side1 - side2) * tsize // normal // t3
    vertices.push_back({(normal + side1 - side2) * tsize, normal, TexCoordss[3]});
  }

  // Build RH above
  if (!rhcoords)
    ReverseWinding(indices, vertices);

  if (invertn)
    InvertNormals(vertices);
}

//--------------------------------------------------------------------------------------
// Sphere
//--------------------------------------------------------------------------------------
void ComputeSphere(std::vector<Vertex> &vertices,
                   std::vector<unsigned int> &indices,
                   float diameter,
                   size_t tessellation,
                   bool rhcoords,
                   bool invertn) {
  vertices.clear();
  indices.clear();

  if (tessellation < 3)
    throw std::invalid_argument("tesselation parameter must be at least 3");

  const size_t verticalSegments = tessellation;
  const size_t horizontalSegments = tessellation * 2;

  const float radius = diameter / 2;

  // Create rings of vertices at progressively higher latitudes.
  for (size_t i = 0; i <= verticalSegments; i++) {
    const float v = 1 - float(i) / float(verticalSegments);

    const float latitude = (float(i) * Shape_PI / float(verticalSegments)) - Shape_PIDIV2;
    float dy, dxz;

    XMScalarSinCos(&dy, &dxz, latitude);

    // Create a single ring of vertices at this latitude.
    for (size_t j = 0; j <= horizontalSegments; j++) {
      const float u = float(j) / float(horizontalSegments);

      const float longitude = float(j) * Shape_2PI / float(horizontalSegments);
      float dx, dz;

      XMScalarSinCos(&dx, &dz, longitude);

      dx *= dxz;
      dz *= dxz;

      const glm::vec3 normal = {dx, dy, dz};
      const glm::vec3 TexCoords = {u, v, 0};

      vertices.push_back({normal * radius, normal, TexCoords});
    }
  }

  // Fill the index buffer with triangles joining each pair of latitude rings.
  const size_t stride = horizontalSegments + 1;

  for (size_t i = 0; i < verticalSegments; i++) {
    for (size_t j = 0; j <= horizontalSegments; j++) {
      const size_t nextI = i + 1;
      const size_t nextJ = (j + 1) % stride;

      index_push_back(indices, i * stride + j);
      index_push_back(indices, nextI * stride + j);
      index_push_back(indices, i * stride + nextJ);

      index_push_back(indices, i * stride + nextJ);
      index_push_back(indices, nextI * stride + j);
      index_push_back(indices, nextI * stride + nextJ);
    }
  }

  // Build RH above
  if (!rhcoords)
    ReverseWinding(indices, vertices);

  if (invertn)
    InvertNormals(vertices);
}

//--------------------------------------------------------------------------------------
// Geodesic sphere
//--------------------------------------------------------------------------------------
void ComputeGeoSphere(std::vector<Vertex> &vertices,
                      std::vector<unsigned int> &indices,
                      float diameter,
                      size_t tessellation,
                      bool rhcoords) {
  vertices.clear();
  indices.clear();

  // An undirected edge between two vertices, represented by a pair of indexes into a vertex array.
  // Becuse this edge is undirected, (a,b) is the same as (b,a).
  using UndirectedEdge = std::pair<unsigned int, unsigned int>;

  // Makes an undirected edge. Rather than overloading comparison operators to give us the (a,b)==(b,a) property,
  // we'll just ensure that the larger of the two goes first. This'll simplify things greatly.
  auto makeUndirectedEdge = [](unsigned int a, unsigned int b) noexcept {
    return std::make_pair(std::max(a, b), std::min(a, b));
  };

  // Key: an edge
  // Value: the index of the vertex which lies midway between the two vertices pointed to by the key value
  // This map is used to avoid duplicating vertices when subdividing triangles along edges.
  using EdgeSubdivisionMap = std::map<UndirectedEdge, unsigned int>;


  static const glm::vec3 OctahedronVertices[] =
      {
          // when looking down the negative z-axis (into the screen)
          glm::vec3(0, 1, 0), // 0 top
          glm::vec3(0, 0, -1), // 1 front
          glm::vec3(1, 0, 0), // 2 right
          glm::vec3(0, 0, 1), // 3 back
          glm::vec3(-1, 0, 0), // 4 left
          glm::vec3(0, -1, 0), // 5 bottom
      };
  static const unsigned int OctahedronIndices[] =
      {
          0, 1, 2, // top front-right face
          0, 2, 3, // top back-right face
          0, 3, 4, // top back-left face
          0, 4, 1, // top front-left face
          5, 1, 4, // bottom front-left face
          5, 4, 3, // bottom back-left face
          5, 3, 2, // bottom back-right face
          5, 2, 1, // bottom front-right face
      };

  const float radius = diameter / 2.0f;

  // Start with an octahedron; copy the data into the vertex/index collection.

  std::vector<glm::vec3> vertexPositions(std::begin(OctahedronVertices), std::end(OctahedronVertices));

  indices.insert(indices.begin(), std::begin(OctahedronIndices), std::end(OctahedronIndices));

  // We know these values by looking at the above index list for the octahedron. Despite the subdivisions that are
  // about to go on, these values aren't ever going to change because the vertices don't move around in the array.
  // We'll need these values later on to fix the singularities that show up at the poles.
  constexpr unsigned int northPoleIndex = 0;
  constexpr unsigned int southPoleIndex = 5;

  for (size_t iSubdivision = 0; iSubdivision < tessellation; ++iSubdivision) {
    assert(indices.size() % 3 == 0); // sanity

    // We use this to keep track of which edges have already been subdivided.
    EdgeSubdivisionMap subdividedEdges;

    // The new index collection after subdivision.
    std::vector<unsigned int> newIndices;

    const size_t triangleCount = indices.size() / 3;
    for (size_t iTriangle = 0; iTriangle < triangleCount; ++iTriangle) {
      // For each edge on this triangle, create a new vertex in the middle of that edge.
      // The winding order of the triangles we output are the same as the winding order of the inputs.

      // Indices of the vertices making up this triangle
      const unsigned int iv0 = indices[iTriangle * 3 + 0];
      const unsigned int iv1 = indices[iTriangle * 3 + 1];
      const unsigned int iv2 = indices[iTriangle * 3 + 2];

      // Get the new vertices
      glm::vec3 v01; // vertex on the midpoint of v0 and v1
      glm::vec3 v12; // ditto v1 and v2
      glm::vec3 v20; // ditto v2 and v0
      unsigned int iv01; // index of v01
      unsigned int iv12; // index of v12
      unsigned int iv20; // index of v20

      // Function that, when given the index of two vertices, creates a new vertex at the midpoint of those vertices.
      auto const divideEdge = [&](unsigned int i0, unsigned int i1, glm::vec3 &outVertex, unsigned int &outIndex) {
        const UndirectedEdge edge = makeUndirectedEdge(i0, i1);

        // Check to see if we've already generated this vertex
        auto it = subdividedEdges.find(edge);
        if (it != subdividedEdges.end()) {
          // We've already generated this vertex before
          outIndex = it->second; // the index of this vertex
          outVertex = vertexPositions[outIndex]; // and the vertex itself
        } else {
          // Haven't generated this vertex before: so add it now

          // outVertex = (vertices[i0] + vertices[i1]) / 2
          outVertex = (vertices[i0].Position + vertices[i1].Position) / 2.0f;

          outIndex = static_cast<unsigned int>(vertexPositions.size());
          CheckIndexOverflow(outIndex);
          vertexPositions.push_back(outVertex);

          // Now add it to the map.
          auto entry = std::make_pair(edge, outIndex);
          subdividedEdges.insert(entry);
        }
      };

      // Add/get new vertices and their indices
      divideEdge(iv0, iv1, v01, iv01);
      divideEdge(iv1, iv2, v12, iv12);
      divideEdge(iv0, iv2, v20, iv20);

      // Add the new indices. We have four new triangles from our original one:
      //        v0
      //        o
      //       /a\
            //  v20 o---o v01
      //     /b\c/d\
            // v2 o---o---o v1
      //       v12
      const unsigned int indicesToAdd[] =
          {
              iv0, iv01, iv20, // a
              iv20, iv12, iv2, // b
              iv20, iv01, iv12, // c
              iv01, iv1, iv12, // d
          };
      newIndices.insert(newIndices.end(), std::begin(indicesToAdd), std::end(indicesToAdd));
    }

    indices = std::move(newIndices);
  }

  // Now that we've completed subdivision, fill in the final vertex collection
  vertices.reserve(vertexPositions.size());
  for (const auto &it : vertexPositions) {
    auto const normal = glm::normalize(it);
    auto const pos = normal * radius;

    glm::vec3 normalFloat3 = normal;

    // calculate texture coordinates for this vertex
    const float longitude = atan2f(normalFloat3.x, -normalFloat3.z);
    const float latitude = acosf(normalFloat3.y);

    const float u = longitude / Shape_2PI + 0.5f;
    const float v = latitude / Shape_PI;

    const glm::vec3 texcoord = {1.0f - u, v, 0.0f};
    vertices.push_back({pos, normal, texcoord});
  }

  // There are a couple of fixes to do. One is a texture coordinate wraparound fixup. At some point, there will be
  // a set of triangles somewhere in the mesh with texture coordinates such that the wraparound across 0.0/1.0
  // occurs across that triangle. Eg. when the left hand side of the triangle has a U coordinate of 0.98 and the
  // right hand side has a U coordinate of 0.0. The intent is that such a triangle should render with a U of 0.98 to
  // 1.0, not 0.98 to 0.0. If we don't do this fixup, there will be a visible seam across one side of the sphere.
  //
  // Luckily this is relatively easy to fix. There is a straight edge which runs down the prime meridian of the
  // completed sphere. If you imagine the vertices along that edge, they circumscribe a semicircular arc starting at
  // y=1 and ending at y=-1, and sweeping across the range of z=0 to z=1. x stays zero. It's along this edge that we
  // need to duplicate our vertices - and provide the correct texture coordinates.
  const size_t preFixupVertexCount = vertices.size();
  for (size_t i = 0; i < preFixupVertexCount; ++i) {
    // This vertex is on the prime meridian if position.x and texcoord.u are both zero (allowing for small epsilon).
    const bool isOnPrimeMeridian =
        std::abs(vertices[i].Position.x) <= 1.192092896e-7 && std::abs(vertices[i].TexCoords.x) <= 1.192092896e-7;

    if (isOnPrimeMeridian) {
      size_t newIndex = vertices.size(); // the index of this vertex that we're about to add
      CheckIndexOverflow(newIndex);

      // copy this vertex, correct the texture coordinate, and add the vertex
      Vertex v = vertices[i];
      v.TexCoords.x = 1.0f;
      vertices.push_back(v);

      // Now find all the triangles which contain this vertex and update them if necessary
      for (size_t j = 0; j < indices.size(); j += 3) {
        unsigned int *triIndex0 = &indices[j + 0];
        unsigned int *triIndex1 = &indices[j + 1];
        unsigned int *triIndex2 = &indices[j + 2];

        if (*triIndex0 == i) {
          // nothing; just keep going
        } else if (*triIndex1 == i) {
          std::swap(triIndex0, triIndex1); // swap the pointers (not the values)
        } else if (*triIndex2 == i) {
          std::swap(triIndex0, triIndex2); // swap the pointers (not the values)
        } else {
          // this triangle doesn't use the vertex we're interested in
          continue;
        }

        // If we got to this point then triIndex0 is the pointer to the index to the vertex we're looking at
        assert(*triIndex0 == i);
        assert(*triIndex1 != i && *triIndex2 != i); // assume no degenerate triangles

        const Vertex &v0 = vertices[*triIndex0];
        const Vertex &v1 = vertices[*triIndex1];
        const Vertex &v2 = vertices[*triIndex2];

        // check the other two vertices to see if we might need to fix this triangle

        if (abs(v0.TexCoords.x - v1.TexCoords.x) > 0.5f ||
            abs(v0.TexCoords.x - v2.TexCoords.x) > 0.5f) {
          // yep; replace the specified index to point to the new, corrected vertex
          *triIndex0 = static_cast<unsigned int>(newIndex);
        }
      }
    }
  }

  // And one last fix we need to do: the poles. A common use-case of a sphere mesh is to map a rectangular texture onto
  // it. If that happens, then the poles become singularities which map the entire top and bottom rows of the texture
  // onto a single point. In general there's no real way to do that right. But to match the behavior of non-geodesic
  // spheres, we need to duplicate the pole vertex for every triangle that uses it. This will introduce seams near the
  // poles, but reduce stretching.
  auto const fixPole = [&](size_t poleIndex) {
    const auto &poleVertex = vertices[poleIndex];
    bool overwrittenPoleVertex = false; // overwriting the original pole vertex saves us one vertex

    for (size_t i = 0; i < indices.size(); i += 3) {
      // These pointers point to the three indices which make up this triangle. pPoleIndex is the pointer to the
      // entry in the index array which represents the pole index, and the other two pointers point to the other
      // two indices making up this triangle.
      unsigned int *pPoleIndex;
      unsigned int *pOtherIndex0;
      unsigned int *pOtherIndex1;
      if (indices[i + 0] == poleIndex) {
        pPoleIndex = &indices[i + 0];
        pOtherIndex0 = &indices[i + 1];
        pOtherIndex1 = &indices[i + 2];
      } else if (indices[i + 1] == poleIndex) {
        pPoleIndex = &indices[i + 1];
        pOtherIndex0 = &indices[i + 2];
        pOtherIndex1 = &indices[i + 0];
      } else if (indices[i + 2] == poleIndex) {
        pPoleIndex = &indices[i + 2];
        pOtherIndex0 = &indices[i + 0];
        pOtherIndex1 = &indices[i + 1];
      } else {
        continue;
      }

      const auto &otherVertex0 = vertices[*pOtherIndex0];
      const auto &otherVertex1 = vertices[*pOtherIndex1];

      // Calculate the texcoords for the new pole vertex, add it to the vertices and update the index
      Vertex newPoleVertex = poleVertex;
      newPoleVertex.TexCoords.x = (otherVertex0.TexCoords.x + otherVertex1.TexCoords.x) / 2;
      newPoleVertex.TexCoords.y = poleVertex.TexCoords.y;

      if (!overwrittenPoleVertex) {
        vertices[poleIndex] = newPoleVertex;
        overwrittenPoleVertex = true;
      } else {
        CheckIndexOverflow(vertices.size());

        *pPoleIndex = static_cast<unsigned int>(vertices.size());
        vertices.push_back(newPoleVertex);
      }
    }
  };

  fixPole(northPoleIndex);
  fixPole(southPoleIndex);

  // Build RH above
  if (!rhcoords)
    ReverseWinding(indices, vertices);
}


//--------------------------------------------------------------------------------------
// Cylinder / Cone
//--------------------------------------------------------------------------------------
namespace {
// Helper computes a point on a unit circle, aligned to the x/z plane and centered on the origin.
inline glm::vec4 GetCircleVector(size_t i, size_t tessellation) noexcept {
  const float angle = float(i) * Shape_2PI / float(tessellation);
  float dx, dz;

  XMScalarSinCos(&dx, &dz, angle);

  const glm::vec4 v = {dx, 0, dz, 0};
  return v;
}

inline glm::vec3 GetCircleTangent(size_t i, size_t tessellation) noexcept {
  const float angle = (float(i) * Shape_2PI / float(tessellation)) + Shape_PIDIV2;
  float dx, dz;

  XMScalarSinCos(&dx, &dz, angle);

  const glm::vec4 v = {dx, 0, dz, 0};
  return v;
}

// Helper creates a triangle fan to close the end of a cylinder / cone
void CreateCylinderCap(std::vector<Vertex> &vertices,
                       std::vector<unsigned int> &indices,
                       size_t tessellation,
                       float height,
                       float radius,
                       bool isTop) {
  // Create cap indices.
  for (size_t i = 0; i < tessellation - 2; i++) {
    size_t i1 = (i + 1) % tessellation;
    size_t i2 = (i + 2) % tessellation;

    if (isTop) {
      std::swap(i1, i2);
    }

    const size_t vbase = vertices.size();
    index_push_back(indices, vbase);
    index_push_back(indices, vbase + i1);
    index_push_back(indices, vbase + i2);
  }

  // Which end of the cylinder is this?
  glm::vec4 normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
  glm::vec4 textureScale = {-0.5f, -0.5f, -0.5f, -0.5f};

  if (!isTop) {
    normal = -normal;
    textureScale = textureScale * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
  }

  // Create cap vertices.
  for (size_t i = 0; i < tessellation; i++) {
    const glm::vec4 circleVector = GetCircleVector(i, tessellation);

    const glm::vec4 position = (circleVector * radius + normal * height);

    const glm::vec3 TexCoords = glm::vec4(circleVector.x, circleVector.z, circleVector.w, circleVector.w) * textureScale
        + glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);

    vertices.push_back({position, normal, TexCoords});
  }
}
}

void ComputeCylinder(std::vector<Vertex> &vertices,
                     std::vector<unsigned int> &indices,
                     float height,
                     float diameter,
                     size_t tessellation,
                     bool rhcoords) {
  vertices.clear();
  indices.clear();

  if (tessellation < 3)
    throw std::invalid_argument("tesselation parameter must be at least 3");

  height /= 2;

  const glm::vec3 topOffset = glm::vec3(0.0f, 1.0f, 0.0f) * height;

  const float radius = diameter / 2;
  const size_t stride = tessellation + 1;

  // Create a ring of triangles around the outside of the cylinder.
  for (size_t i = 0; i <= tessellation; i++) {
    const glm::vec3 normal = GetCircleVector(i, tessellation);

    const glm::vec3 sideOffset = normal * radius;

    const float u = float(i) / float(tessellation);

    const glm::vec3 TexCoords = {u, 0, 0};

    vertices.push_back({sideOffset + topOffset, normal, TexCoords});
    vertices.push_back({(sideOffset - topOffset), normal, TexCoords + glm::vec3(0.0f, 1.0f, 0.0f)});

    index_push_back(indices, i * 2);
    index_push_back(indices, (i * 2 + 2) % (stride * 2));
    index_push_back(indices, i * 2 + 1);

    index_push_back(indices, i * 2 + 1);
    index_push_back(indices, (i * 2 + 2) % (stride * 2));
    index_push_back(indices, (i * 2 + 3) % (stride * 2));
  }

  // Create flat triangle fan caps to seal the top and bottom.
  CreateCylinderCap(vertices, indices, tessellation, height, radius, true);
  CreateCylinderCap(vertices, indices, tessellation, height, radius, false);

  // Build RH above
  if (!rhcoords)
    ReverseWinding(indices, vertices);
}

// Creates a cone primitive.
void ComputeCone(std::vector<Vertex> &vertices,
                 std::vector<unsigned int> &indices,
                 float diameter,
                 float height,
                 size_t tessellation,
                 bool rhcoords) {
  vertices.clear();
  indices.clear();

  if (tessellation < 3)
    throw std::invalid_argument("tesselation parameter must be at least 3");

  height /= 2;

  const glm::vec3 topOffset = glm::vec3(0.0f, 1.0f, 0.0f) * height;

  const float radius = diameter / 2;
  const size_t stride = tessellation + 1;

  // Create a ring of triangles around the outside of the cone.
  for (size_t i = 0; i <= tessellation; i++) {
    const glm::vec3 circlevec = GetCircleVector(i, tessellation);

    const glm::vec3 sideOffset = circlevec * radius;

    const float u = float(i) / float(tessellation);

    const glm::vec3 TexCoords = {u, 0, 0};

    const glm::vec3 pt = sideOffset - topOffset;

    glm::vec3 normal = glm::cross(
        GetCircleTangent(i, tessellation),
        (topOffset - pt));
    normal = glm::normalize(normal);

    // Duplicate the top vertex for distinct normals
    vertices.push_back({topOffset, normal, {0, 0}});
    vertices.push_back({pt, normal, TexCoords + glm::vec3(0.0f, 1.0f, 0.0f)});

    index_push_back(indices, i * 2);
    index_push_back(indices, (i * 2 + 3) % (stride * 2));
    index_push_back(indices, (i * 2 + 1) % (stride * 2));
  }

  // Create flat triangle fan caps to seal the bottom.
  CreateCylinderCap(vertices, indices, tessellation, height, radius, false);

  // Build RH above
  if (!rhcoords)
    ReverseWinding(indices, vertices);
}

//--------------------------------------------------------------------------------------
// Torus
//--------------------------------------------------------------------------------------
void ComputeTorus(std::vector<Vertex> &vertices,
                  std::vector<unsigned int> &indices,
                  float diameter,
                  float thickness,
                  size_t tessellation,
                  bool rhcoords) {
  vertices.clear();
  indices.clear();

  if (tessellation < 3)
    throw std::invalid_argument("tesselation parameter must be at least 3");

  const size_t stride = tessellation + 1;

  // First we loop around the main ring of the torus.
  for (size_t i = 0; i <= tessellation; i++) {
    const float u = float(i) / float(tessellation);

    const float outerAngle = float(i) * Shape_2PI / float(tessellation) - Shape_PIDIV2;

    // Create a transform matrix that will align geometry to
    // slice perpendicularly though the current ring position.
    const glm::mat4
        transform = glm::translate(glm::vec3(diameter / 2, 0, 0)) * glm::rotate(outerAngle, glm::vec3(0, 1, 0));

    // Now we loop along the other axis, around the side of the tube.
    for (size_t j = 0; j <= tessellation; j++) {
      const float v = 1 - float(j) / float(tessellation);

      const float innerAngle = float(j) * Shape_2PI / float(tessellation) + Shape_PI;
      float dx, dy;

      XMScalarSinCos(&dy, &dx, innerAngle);

      // Create a vertex.
      glm::vec4 normal = {dx, dy, 0, 0};
      glm::vec4 position = normal * thickness / 2.0f;
      const glm::vec3 TexCoords = {u, v, 0};

      position = transform * position;
      normal = glm::rotate(outerAngle, glm::vec3(0, 1, 0)) * normal;

      vertices.push_back({position, normal, TexCoords});

      // And create indices for two triangles.
      const size_t nextI = (i + 1) % stride;
      const size_t nextJ = (j + 1) % stride;

      index_push_back(indices, i * stride + j);
      index_push_back(indices, i * stride + nextJ);
      index_push_back(indices, nextI * stride + j);

      index_push_back(indices, i * stride + nextJ);
      index_push_back(indices, nextI * stride + nextJ);
      index_push_back(indices, nextI * stride + j);
    }
  }

  // Build RH above
  if (!rhcoords)
    ReverseWinding(indices, vertices);
}

//--------------------------------------------------------------------------------------
// Tetrahedron
//--------------------------------------------------------------------------------------
void ComputeTetrahedron(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, float size, bool rhcoords) {
  vertices.clear();
  indices.clear();

  static const glm::vec4 verts[4] =
      {
          {0.f, 0.f, 1.f, 0},
          {2.f * SQRT2 / 3.f, 0.f, -1.f / 3.f, 0},
          {-SQRT2 / 3.f, SQRT6 / 3.f, -1.f / 3.f, 0},
          {-SQRT2 / 3.f, -SQRT6 / 3.f, -1.f / 3.f, 0}
      };

  static const uint32_t faces[4 * 3] =
      {
          0, 1, 2,
          0, 2, 3,
          0, 3, 1,
          1, 3, 2,
      };

  for (size_t j = 0; j < std::size(faces); j += 3) {
    const uint32_t v0 = faces[j];
    const uint32_t v1 = faces[j + 1];
    const uint32_t v2 = faces[j + 2];

    glm::vec3 normal = glm::cross(glm::vec3(verts[v1] - verts[v0]), glm::vec3(verts[v2] - verts[v0]));
    normal = glm::normalize(normal);

    const size_t base = vertices.size();
    index_push_back(indices, base);
    index_push_back(indices, base + 1);
    index_push_back(indices, base + 2);

    // Duplicate vertices to use face normals
    glm::vec3 position = verts[v0] * size;
    vertices.push_back({position, normal, {0, 0}});

    position = verts[v1] * size;
    vertices.push_back({position, normal, {1, 0}});

    position = verts[v2] * size;
    vertices.push_back({position, normal, {0, 1}});
  }

  // Built LH above
  if (rhcoords)
    ReverseWinding(indices, vertices);

  assert(vertices.size() == 4 * 3);
  assert(indices.size() == 4 * 3);
}

//--------------------------------------------------------------------------------------
// Octahedron
//--------------------------------------------------------------------------------------
void ComputeOctahedron(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, float size, bool rhcoords) {
  vertices.clear();
  indices.clear();

  static const glm::vec4 verts[6] =
      {
          {1, 0, 0, 0},
          {-1, 0, 0, 0},
          {0, 1, 0, 0},
          {0, -1, 0, 0},
          {0, 0, 1, 0},
          {0, 0, -1, 0}
      };

  static const uint32_t faces[8 * 3] =
      {
          4, 0, 2,
          4, 2, 1,
          4, 1, 3,
          4, 3, 0,
          5, 2, 0,
          5, 1, 2,
          5, 3, 1,
          5, 0, 3
      };

  for (size_t j = 0; j < std::size(faces); j += 3) {
    const uint32_t v0 = faces[j];
    const uint32_t v1 = faces[j + 1];
    const uint32_t v2 = faces[j + 2];

    glm::vec3 normal = glm::cross(glm::vec3(verts[v1] - verts[v0]), glm::vec3(verts[v2] - verts[v0]));
    normal = glm::normalize(normal);

    const size_t base = vertices.size();
    index_push_back(indices, base);
    index_push_back(indices, base + 1);
    index_push_back(indices, base + 2);

    // Duplicate vertices to use face normals
    glm::vec3 position = verts[v0] * size;
    vertices.push_back({position, normal, {0, 0}});

    position = verts[v1] * size;
    vertices.push_back({position, normal, {1, 0} /* 1, 0 */});

    position = verts[v2] * size;
    vertices.push_back({position, normal, {0, 1}});
  }

  // Built LH above
  if (rhcoords)
    ReverseWinding(indices, vertices);

  assert(vertices.size() == 8 * 3);
  assert(indices.size() == 8 * 3);
}

//--------------------------------------------------------------------------------------
// Dodecahedron
//--------------------------------------------------------------------------------------
void ComputeDodecahedron(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, float size, bool rhcoords) {
  vertices.clear();
  indices.clear();

  constexpr float a = 1.f / SQRT3;
  constexpr float b = 0.356822089773089931942f; // sqrt( ( 3 - sqrt(5) ) / 6 )
  constexpr float c = 0.934172358962715696451f; // sqrt( ( 3 + sqrt(5) ) / 6 );

  static const glm::vec4 verts[20] =
      {
          {a, a, a, 0},
          {a, a, -a, 0},
          {a, -a, a, 0},
          {a, -a, -a, 0},
          {-a, a, a, 0},
          {-a, a, -a, 0},
          {-a, -a, a, 0},
          {-a, -a, -a, 0},
          {b, c, 0, 0},
          {-b, c, 0, 0},
          {b, -c, 0, 0},
          {-b, -c, 0, 0},
          {c, 0, b, 0},
          {c, 0, -b, 0},
          {-c, 0, b, 0},
          {-c, 0, -b, 0},
          {0, b, c, 0},
          {0, -b, c, 0},
          {0, b, -c, 0},
          {0, -b, -c, 0}
      };

  static const uint32_t faces[12 * 5] =
      {
          0, 8, 9, 4, 16,
          0, 16, 17, 2, 12,
          12, 2, 10, 3, 13,
          9, 5, 15, 14, 4,
          3, 19, 18, 1, 13,
          7, 11, 6, 14, 15,
          0, 12, 13, 1, 8,
          8, 1, 18, 5, 9,
          16, 4, 14, 6, 17,
          6, 11, 10, 2, 17,
          7, 15, 5, 18, 19,
          7, 19, 3, 10, 11,
      };

  static const glm::vec4 TexCoordss[5] =
      {
          {0.654508f, 0.0244717f, 0, 0},
          {0.0954915f, 0.206107f, 0, 0},
          {0.0954915f, 0.793893f, 0, 0},
          {0.654508f, 0.975528f, 0, 0},
          {1.f, 0.5f, 0, 0}
      };

  static const uint32_t textureIndex[12][5] =
      {
          {0, 1, 2, 3, 4},
          {2, 3, 4, 0, 1},
          {4, 0, 1, 2, 3},
          {1, 2, 3, 4, 0},
          {2, 3, 4, 0, 1},
          {0, 1, 2, 3, 4},
          {1, 2, 3, 4, 0},
          {4, 0, 1, 2, 3},
          {4, 0, 1, 2, 3},
          {1, 2, 3, 4, 0},
          {0, 1, 2, 3, 4},
          {2, 3, 4, 0, 1},
      };

  size_t t = 0;
  for (size_t j = 0; j < std::size(faces); j += 5, ++t) {
    const uint32_t v0 = faces[j];
    const uint32_t v1 = faces[j + 1];
    const uint32_t v2 = faces[j + 2];
    const uint32_t v3 = faces[j + 3];
    const uint32_t v4 = faces[j + 4];

    glm::vec3 normal = glm::cross(glm::vec3(verts[v1] - verts[v0]), glm::vec3(verts[v2] - verts[v0]));
    normal = glm::normalize(normal);

    const size_t base = vertices.size();

    index_push_back(indices, base);
    index_push_back(indices, base + 1);
    index_push_back(indices, base + 2);

    index_push_back(indices, base);
    index_push_back(indices, base + 2);
    index_push_back(indices, base + 3);

    index_push_back(indices, base);
    index_push_back(indices, base + 3);
    index_push_back(indices, base + 4);

    // Duplicate vertices to use face normals
    glm::vec3 position = verts[v0] * size;
    vertices.push_back({position, normal, TexCoordss[textureIndex[t][0]]});

    position = verts[v1] * size;
    vertices.push_back({position, normal, TexCoordss[textureIndex[t][1]]});

    position = verts[v2] * size;
    vertices.push_back({position, normal, TexCoordss[textureIndex[t][2]]});

    position = verts[v3] * size;
    vertices.push_back({position, normal, TexCoordss[textureIndex[t][3]]});

    position = verts[v4] * size;
    vertices.push_back({position, normal, TexCoordss[textureIndex[t][4]]});
  }

  // Built LH above
  if (rhcoords)
    ReverseWinding(indices, vertices);

  assert(vertices.size() == 12 * 5);
  assert(indices.size() == 12 * 3 * 3);
}

//--------------------------------------------------------------------------------------
// Icosahedron
//--------------------------------------------------------------------------------------
void ComputeIcosahedron(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, float size, bool rhcoords) {
  vertices.clear();
  indices.clear();

  constexpr float t = 1.618033988749894848205f; // (1 + sqrt(5)) / 2
  constexpr float t2 = 1.519544995837552493271f; // sqrt( 1 + sqr( (1 + sqrt(5)) / 2 ) )

  static const glm::vec4 verts[12] =
      {
          {t / t2, 1.f / t2, 0, 0},
          {-t / t2, 1.f / t2, 0, 0},
          {t / t2, -1.f / t2, 0, 0},
          {-t / t2, -1.f / t2, 0, 0},
          {1.f / t2, 0, t / t2, 0},
          {1.f / t2, 0, -t / t2, 0},
          {-1.f / t2, 0, t / t2, 0},
          {-1.f / t2, 0, -t / t2, 0},
          {0, t / t2, 1.f / t2, 0},
          {0, -t / t2, 1.f / t2, 0},
          {0, t / t2, -1.f / t2, 0},
          {0, -t / t2, -1.f / t2, 0}
      };

  static const uint32_t faces[20 * 3] =
      {
          0, 8, 4,
          0, 5, 10,
          2, 4, 9,
          2, 11, 5,
          1, 6, 8,
          1, 10, 7,
          3, 9, 6,
          3, 7, 11,
          0, 10, 8,
          1, 8, 10,
          2, 9, 11,
          3, 11, 9,
          4, 2, 0,
          5, 0, 2,
          6, 1, 3,
          7, 3, 1,
          8, 6, 4,
          9, 4, 6,
          10, 5, 7,
          11, 7, 5
      };

  for (size_t j = 0; j < std::size(faces); j += 3) {
    const uint32_t v0 = faces[j];
    const uint32_t v1 = faces[j + 1];
    const uint32_t v2 = faces[j + 2];

    glm::vec3 normal = glm::cross(glm::vec3(verts[v1] - verts[v0]), glm::vec3(verts[v2] - verts[v0]));
    normal = glm::normalize(normal);

    const size_t base = vertices.size();
    index_push_back(indices, base);
    index_push_back(indices, base + 1);
    index_push_back(indices, base + 2);

    // Duplicate vertices to use face normals
    glm::vec3 position = verts[v0] * size;
    vertices.push_back({position, normal, {0, 0} /* 0, 0 */});

    position = verts[v1] * size;
    vertices.push_back({position, normal, {1, 0}});

    position = verts[v2] * size;
    vertices.push_back({position, normal, {0, 1}});
  }

  // Built LH above
  if (rhcoords)
    ReverseWinding(indices, vertices);

  assert(vertices.size() == 20 * 3);
  assert(indices.size() == 20 * 3);
}
void ComputeTangentBasis(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices) {
  for (auto &v : vertices) {
    v.Tangent = glm::vec3(0);
    v.Bitangent = glm::vec3(0);
  }
  for (int i = 0; i < indices.size(); i += 3) {
    auto ind1 = indices[i];
    auto ind2 = indices[i + 1];
    auto ind3 = indices[i + 2];
    glm::vec3 &v0 = vertices[ind1].Position;
    glm::vec3 &v1 = vertices[ind2].Position;
    glm::vec3 &v2 = vertices[ind3].Position;

    // Shortcuts for UVs
    glm::vec2 &uv0 = vertices[ind1].TexCoords;
    glm::vec2 &uv1 = vertices[ind2].TexCoords;
    glm::vec2 &uv2 = vertices[ind3].TexCoords;

    // Edges of the triangle : postion delta
    glm::vec3 deltaPos1 = v1 - v0;
    glm::vec3 deltaPos2 = v2 - v0;

    // UV delta
    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;

    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

    vertices[ind1].Tangent += tangent;
    vertices[ind1].Bitangent += bitangent;
    vertices[ind2].Tangent += tangent;
    vertices[ind2].Bitangent += bitangent;
    vertices[ind3].Tangent += tangent;
    vertices[ind3].Bitangent += bitangent;
  }
  for (auto &v : vertices) {
    v.Tangent = glm::normalize(v.Tangent);
    v.Bitangent = glm::normalize(v.Bitangent);
  }
}


