//
// Created by Lenovo on 2022/4/26.
//

#include "texture.h"

#include <iostream>
#include <filesystem>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb_image_write.h>
Texture::Texture(
                 const string &path)
    : path(path) {

  void *load_data;
  int nrComponents;
    load_data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

  if (load_data) {
      data = malloc(width * height * nrComponents * sizeof(unsigned char));
      memcpy(data, load_data, sizeof(unsigned char) * width * height * nrComponents);
    stbi_image_free(load_data);
  }
  if (data != nullptr) {
    if (nrComponents == 1) {
      format = GL_RED;
      internalFormat = GL_RED;
    } else if (nrComponents == 3) {
      format = GL_RGB;
      internalFormat = GL_RGB;
    } else if (nrComponents == 4) {
      format = GL_RGBA;
      internalFormat = GL_RGBA;
    }
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
  }
}
void Texture::SetupGL() {
  glGenTextures(1, &glResourceID);
  glBindTexture(GL_TEXTURE_2D, glResourceID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, data);
    if(generateMips){
    glGenerateMipmap(GL_TEXTURE_2D);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrapParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrapParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
}

Texture::Texture() {}
Texture::~Texture() {
  if (data != nullptr) {
    free(data);
    data = nullptr;
  }
}
Texture::Texture(int width,
                 int height,
                 GLenum format,
                 GLint internal_format,
                 GLint s_wrap_param,
                 GLint t_wrap_param,
                 GLenum data_type,
                 GLint min_filter,
                 GLint max_filter,bool generateMips)
    : width(width),
      height(height),
      format(format),
      internalFormat(internal_format),
      sWrapParam(s_wrap_param),
      tWrapParam(t_wrap_param),
      dataType(data_type),
      minFilter(min_filter),
      maxFilter(max_filter),generateMips(generateMips) {}
void Texture::Save(string path) {

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
//  stbi_write_png(path.c_str(), width, height, nrChannels, buffer.data(), stride);
}
HDRTexture::HDRTexture(const string &path)  {
  this->path = path;
  void *load_data;
  int nrComponents;
  load_data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
  if (load_data){
    data = malloc(width * height * nrComponents * sizeof(float));
    memcpy(data, load_data, sizeof(float) * width * height * nrComponents);
    stbi_image_free(load_data);
  }
  if (data == nullptr) {
    std::cout << "Texture failed to load at path: " << path << std::endl;
  }
  format = GL_RGB;
  internalFormat = GL_RGB32F;dataType=GL_FLOAT;
  sWrapParam = GL_CLAMP_TO_EDGE;
  tWrapParam = GL_CLAMP_TO_EDGE;
  minFilter = GL_LINEAR;
  maxFilter = GL_LINEAR;
}
