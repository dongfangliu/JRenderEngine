//
// Created by Lenovo on 2022/4/26.
//

#ifndef JRENDERENGINE_INCLUDE_TEXTURE_H_
#define JRENDERENGINE_INCLUDE_TEXTURE_H_
#include <string>
#include <vector>
#include <memory>
#include "glad/glad.h"
using namespace  std;
class Texture {
 public:
  unsigned int id;
  string path;
  void *data = nullptr;
  int width,height, nrComponents;
  GLenum format;
  GLenum internalFormat;
  bool sRGB = false;
  bool HDR = false;
 public:
  Texture(const string &path,bool sRGB =false);
  Texture();
   ~Texture();
  virtual void SetupGL() ;
};

#endif //JRENDERENGINE_INCLUDE_TEXTURE_H_
