//
// Created by Lenovo on 2022/4/26.
//

#ifndef JRENDERENGINE_INCLUDE_TEXTURE_H_
#define JRENDERENGINE_INCLUDE_TEXTURE_H_
#include <string>
#include <vector>
#include "glad/glad.h"
#include <assimp/material.h>
#include <assimp/pbrmaterial.h>
using namespace  std;
class Texture {
 public:
  unsigned int id;
  aiTextureType type;
  string path;
 private:
  vector<unsigned char>data;
  int width,height, nrComponents;
  GLenum format;
 public:
  Texture(const aiTextureType &type, const string &path);
  Texture();
   ~Texture();

  void SetupGL() ;
};

#endif //JRENDERENGINE_INCLUDE_TEXTURE_H_
