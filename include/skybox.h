//
// Created by Lenovo on 2022/6/6.
//

#ifndef JRENDERENGINE_INCLUDE_SKYBOX_H_
#define JRENDERENGINE_INCLUDE_SKYBOX_H_
#include "cubemap.h"
class Skybox {

  shared_ptr<CubeMap> map;
  static shared_ptr<Shader> shader;
 public:
  explicit Skybox(const shared_ptr<CubeMap> &map);
  void Draw(glm::mat4& projectionMatrix,glm::mat4& viewMatrix);

};

#endif //JRENDERENGINE_INCLUDE_SKYBOX_H_
