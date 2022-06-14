//
// Created by Lenovo on 2022/6/13.
//

#include "shadowmap.h"
#include <glm/gtc/matrix_transform.hpp>
ShadowMap::ShadowMap() {}
ShadowMap::ShadowMap(int width, int height) : Texture(width,
                                                      height,
                                                      GL_DEPTH_COMPONENT,
                                                      GL_DEPTH_COMPONENT,
                                                      GL_CLAMP_TO_BORDER,
                                                      GL_CLAMP_TO_BORDER,
                                                      GL_FLOAT,
                                                      GL_NEAREST,
                                                      GL_NEAREST,
                                                      false) {}
void ShadowMap::SetupGL() {
  Texture::SetupGL();
  GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glGenFramebuffers(1, &this->depthMapFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->glResourceID, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void ShadowMap::BeforeDraw() {
  depthShader.use();

  glViewport(0, 0, width, height);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
//  RenderScene(simpleDepthShader);

}
void ShadowMap::EndDraw() {

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
