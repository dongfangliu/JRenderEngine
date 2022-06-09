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
  BRDFLUT(unsigned int resolution) {

   this->width = resolution;
   this->height = resolution;
 }
  void SetupGL() override;

  void CalculateLUT();
};

#endif //JRENDERENGINE_INCLUDE_BRDFLUT_H_
