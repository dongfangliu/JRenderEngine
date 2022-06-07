//
// Created by Lenovo on 2022/4/25.
//
#include "model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
Model::Model(const string &path, bool gamma) : gammaCorrection(gamma) {
  loadModel(path);
}
void Model::Draw(Shader &shader,int IBLDiffuseIrradianceMapId=-1) {
  for (auto &mesh : meshes) {
    auto &material = materials[mesh.materialIndex];
    shader.setVec4("baseColorFactor", material.baseColorFactor);
    shader.setFloat("metallicFactor", material.metallicFactor);
    shader.setFloat("roughnessFactor", material.roughnessFactor);
    shader.setInt("baseColorMode", material.bcMode);
    shader.setInt("metallicMode", material.metallicMode);
    shader.setInt("roughnessMode", material.roughnessMode);
    if (!material.baseColorTexture.empty()) {
      shader.setInt("baseColorMap", 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, textures_loaded[material.baseColorTexture]->id);
    }
    if (!material.normalTexture.empty()) {
      shader.setInt("normalMap", 1);
      shader.setBool("useNormalMap", true);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, textures_loaded[material.normalTexture]->id);
    } else {
      shader.setBool("useNormalMap", false);
    }
    if (!material.metallicRoughnessTexture.empty()) {
      shader.setInt("metallicRoughnessMap", 2);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, textures_loaded[material.metallicRoughnessTexture]->id);
    }
    if(IBLDiffuseIrradianceMapId!=-1){
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_CUBE_MAP, IBLDiffuseIrradianceMapId);
    }
    shader.setInt("alphaMode", material.alphaMode);
    shader.setFloat("alphaCutOff", material.alphaCutOff);

    if (material.doubleSided) {
      glDisable(GL_CULL_FACE);
    } else {
      glEnable(GL_CULL_FACE);
    }
    mesh.Draw();
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_CULL_FACE);
  }
}
void Model::loadModel(const string &path_str) {
// read in path
  auto p = filesystem::path(path_str);
  string path;
  if (p.is_relative()) {
    p = std::filesystem::canonical(filesystem::current_path() / p);
  }
  path = p.string();
  directory = p.parent_path().string();

  // read file via ASSIMP
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
  // check for errors
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
  {
    cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
    return;
  }
  processMaterials(scene);
  // process ASSIMP's root node recursively
  processNode(scene->mRootNode, scene);
}
void Model::processNode(aiNode *node, const aiScene *scene) {
  // process each mesh located at the current node
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    // the node object only contains indices to index the actual objects in the scene.
    // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }
  // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }

}
void Model::processMaterials(const aiScene *scene) {

  for (unsigned int i = 0; i < scene->mNumMaterials; i++) {

    aiString bc_str, mr_str, normal_str;
    ai_real metallicFactor, roughnessFactor, alphaCutOff;
    aiString alphaMode;
    aiColor4D bc_factor;
    bool doubleSided = false;
    aiMaterial *mat = scene->mMaterials[i];
    mat->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &bc_str);
    mat->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &mr_str);
    mat->GetTexture(aiTextureType_NORMALS, 0, &normal_str);
    mat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, bc_factor);
    mat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, metallicFactor);
    mat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, roughnessFactor);
    mat->Get(AI_MATKEY_GLTF_ALPHACUTOFF, alphaCutOff);
    mat->Get(AI_MATKEY_GLTF_ALPHAMODE, alphaMode);
    mat->Get(AI_MATKEY_TWOSIDED, doubleSided);

    this->materials.emplace_back(glm::vec4(bc_factor.r, bc_factor.g, bc_factor.b, bc_factor.a), metallicFactor,
                               roughnessFactor,
                               bc_str.length > 0 ? directory + "/" + bc_str.C_Str() : "",
                               normal_str.length > 0 ? directory + "/" + normal_str.C_Str() : "",
                               mr_str.length > 0 ? directory + "/" + mr_str.C_Str() : "",
                               string(alphaMode.C_Str()).compare("MASK") ? 0 : 1,
                               alphaCutOff,
                               doubleSided
                              );

    if (bc_str.length > 0) {
      loadMaterialTextures(mat, aiTextureType_DIFFUSE);
    }
    if (mr_str.length > 0) {
      loadMaterialTextures(mat, aiTextureType_UNKNOWN);
    }
    if (bc_str.length > 0) {
      loadMaterialTextures(mat, aiTextureType_NORMALS);
    }

  }

};
Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  // data to fill
  vector<Vertex> vertices;
  vector<unsigned int> indices;
  vector<Texture> textures;

  // walk through each of the mesh's vertices
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex{};
    glm::vec3
        vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
    // positions
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.Position = vector;
    // normals
    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.Normal = vector;
    // texture coordinates
    if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
    {
      glm::vec2 vec;
      // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
      // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.TexCoords = vec;
    } else
      vertex.TexCoords = glm::vec2(0.0f, 0.0f);
    // tangent
    vector.x = mesh->mTangents[i].x;
    vector.y = mesh->mTangents[i].y;
    vector.z = mesh->mTangents[i].z;
    vertex.Tangent = vector;
    // bitangent
    vector.x = mesh->mBitangents[i].x;
    vector.y = mesh->mBitangents[i].y;
    vector.z = mesh->mBitangents[i].z;
    vertex.Bitangent = vector;
    vertices.push_back(vertex);
  }
  // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    // retrieve all indices of the face and store them in the indices vector
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }
  // process materials

  // return a mesh object created from the extracted mesh data
  return {vertices, indices, mesh->mMaterialIndex};
}
vector<shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type) {
  vector<shared_ptr<Texture>> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
    bool skip = false;
    string texture_path = directory + "/" + str.C_Str();
    for (auto &j : textures_loaded) {
      if (std::strcmp(j.second->path.data(), texture_path.data()) == 0 ) {

        textures.push_back(j.second);
        skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
        break;
      }
    }
    if (!skip) {   // if texture hasn't been loaded already, load it
      shared_ptr<Texture>  texture = make_shared<Texture>(texture_path);
      textures.push_back(texture);
      textures_loaded[texture_path] =
          texture;  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
    }
  }
  return textures;
}
void Model::SetupGL() {
  for (auto &kv : textures_loaded) {
    kv.second->SetupGL();
  }
  for(auto& mesh:meshes){mesh.SetupGL();}
}
