//
// Created by Lenovo on 2022/6/9.
//

#include "BRDFLUT.h"
#include "mesh.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
void BRDFLUT::SetupGL() {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
void BRDFLUT::CalculateLUT() {
  unsigned int captureFBO;
  unsigned int captureRBO;
  glGenFramebuffers(1, &captureFBO);
  glGenRenderbuffers(1, &captureRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);

  Shader brdfShader("./shaders/BRDFLUT.vert","./shaders/BRDFLUT.frag");
  glViewport(0, 0, width, height);
  brdfShader.use();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Mesh::GetUnitQuadInstance()->Draw();

//  GLsizei nrChannels = 3;
//  GLsizei stride = nrChannels * width;
//  stride += (stride % 4) ? (4 - stride % 4) : 0;
//  GLsizei bufferSize = stride * height;
//  std::vector<char> buffer(bufferSize);
//  glPixelStorei(GL_PACK_ALIGNMENT, 4);
//  glReadBuffer(GL_FRONT);
//  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
//
//  stbi_flip_vertically_on_write(true);
//  stbi_write_png("lut.png", width, height, nrChannels, buffer.data(), stride);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
