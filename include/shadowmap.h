//
// Created by Lenovo on 2022/6/13.
//

#ifndef JRENDERENGINE_INCLUDE_SHADOWMAP_H_
#define JRENDERENGINE_INCLUDE_SHADOWMAP_H_
#include "texture.h"
#include "shader.h"
#include <glm/glm.hpp>
class ShadowMap : public Texture {
 public:
  GLuint depthMapFBO;
  ShadowMap();
  ShadowMap(int width, int height);
  void SetupGL() override;
  void BeforeDraw();
  void EndDraw();
  Shader depthShader = Shader("./shaders/depthShader.vert", "./shaders/depthShader.frag");
};
#endif //JRENDERENGINE_INCLUDE_SHADOWMAP_H_
