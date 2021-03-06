#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"
#include "material.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <memory>
using namespace std;

class Model {
 public:
  // model data
  map<string, shared_ptr<Texture>>
      textures_loaded;    // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
  vector<Mesh> meshes;
  vector<PBRMaterial> materials;
  string directory;
  bool gammaCorrection;

  // constructor, expects a filepath to a 3D model.
  Model(string const &path, bool gamma = false);

  // draws the model, and thus all its meshes
  void Draw(Shader &shader,
            unsigned int ibl_diffuse_irradiance_map_id = -1,
            unsigned int prefiltered_map_id = -1,
            unsigned int LUT_id = -1,
            unsigned int depth_map_id = -1);
  void PureDraw();
  void SetupGL();
 private:
  // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
  void loadModel(string const &path);

  // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
  void processNode(aiNode *node, const aiScene *scene);

  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  void processMaterials(const aiScene *scene);
  // checks all material textures of a given type and loads the textures if they're not loaded yet.
  // the required info is returned as a Texture struct.
  vector<shared_ptr<Texture>> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

};

#endif
