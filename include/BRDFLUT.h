//
// Created by Lenovo on 2022/6/9.
//

#ifndef JRENDERENGINE_INCLUDE_BRDFLUT_H_
#define JRENDERENGINE_INCLUDE_BRDFLUT_H_
#include "texture.h"
#include "shader.h"
class BRDFLUT:public Texture {

 protected:

 public:
  explicit BRDFLUT(unsigned int resolution) : Texture(resolution,resolution,GL_RG,GL_RG32F,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE){

 }

  void CalculateLUT();
};

#endif //JRENDERENGINE_INCLUDE_BRDFLUT_H_
