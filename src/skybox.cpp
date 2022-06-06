//
// Created by Lenovo on 2022/6/6.
//

#include "skybox.h"
shared_ptr<Shader> Skybox::shader;
Skybox::Skybox(const shared_ptr<CubeMap> &map) : map(map) {
if(Skybox::shader== nullptr){

  Skybox::shader  = make_shared<Shader>("./shaders/skybox.vert","./shaders/skybox.frag");
}
}
void Skybox::Draw(glm::mat4 &projection,glm::mat4 &viewMatrix) {
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_FALSE);
  Skybox::shader->use();
  Skybox::shader->setInt("environmentMap",0);
  Skybox::shader->setMat4("view", viewMatrix);
  Skybox::shader->setMat4("projection", projection);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, map->id) ;
  Mesh::GetUnitSphereInstance()->Draw();
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);
}
