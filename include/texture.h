//
// Created by Lenovo on 2022/4/26.
//

#ifndef JRENDERENGINE_INCLUDE_TEXTURE_H_
#define JRENDERENGINE_INCLUDE_TEXTURE_H_
#include <string>
#include "glad/glad.h"
using namespace  std;
class Texture {
 public:
  unsigned int id;
  string type;
  string path;
 private:
  unsigned char * data;
  int width,height, nrComponents;
  GLenum format;
 public:
  Texture(const string &type, const string &path);
  Texture();
   ~Texture();

  void SetupGL() ;
};

#endif //JRENDERENGINE_INCLUDE_TEXTURE_H_
